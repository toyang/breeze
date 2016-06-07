﻿#include "docker.h"
#include "userMgrService.h"
#include <ProtoCommon.h>
#include <ProtoDBService.h>
#include <ProtoUser.h>

UserMgrService::UserMgrService()
{
    slotting<CreateOrRefreshServiceNotice>(std::bind(&UserMgrService::onCreateOrRefreshServiceNotice, this, _1, _2));
    slotting<RealClientClosedNotice>(std::bind(&UserMgrService::onRealClientClosedNotice, this, _1, _2));
    slotting<SelectUserPreviewsFromUserMgrReq>(std::bind(&UserMgrService::onSelectUserPreviewsFromUserMgrReq, this, _1, _2));
    slotting<CreateUserFromUserMgrReq>(std::bind(&UserMgrService::onCreateUserFromUserMgrReq, this, _1, _2));
    slotting<AttachUserFromUserMgrReq>(std::bind(&UserMgrService::onAttachUserFromUserMgrReq, this, _1, _2));
}

UserMgrService::~UserMgrService()
{
    
}

void UserMgrService::onTick()
{
    time_t now = getNowTime();
    if (now - _lastTime > 10)
    {
        _lastTime = now;
        LOGD("UserMgrService::onTick");
        for (auto iter = _freeList.begin(); iter != _freeList.end();)
        {
            if (!iter->second)
            {
                iter = _freeList.erase(iter);
                continue;
            }
            else if (iter->second->_status == SS_DESTROY || (iter->second->_status == SS_UNINITING && getNowTime() - iter->second->_lastChangeTime > 30))
            {
                if (!Docker::getRef().peekService(ServiceUser, iter->second->_preview.serviceID))
                {
                    iter = _freeList.erase(iter);
                    continue;
                }
                else
                {
                    LOGE("service not uninit finish. used time = " << getNowTime() - iter->second->_lastChangeTime << ", serviceID=" << iter->second->_preview.serviceID);
                }
            }
            iter++;
        }
    }
}

void UserMgrService::onUninit()
{
    _checkSafeDestroy();
}
void UserMgrService::_checkSafeDestroy()
{
    bool safe = true;
    for (auto kv : _userStatusByID)
    {
        if (kv.second->_status == SS_WORKING || kv.second->_status == SS_UNINITING)
        {
            safe = false;
            break;
        }
    }
    if (safe)
    {
        finishUninit();
        return;
    }
    SessionManager::getRef().createTimer(500, std::bind(&UserMgrService::_checkSafeDestroy, this));
}
void UserMgrService::onClientChange()
{
    return ;
}


bool UserMgrService::onInit()
{
    const auto  & config = ServerConfig::getRef().getServiceTypeConfig().at(ServiceUser);
    for (auto dockerID : config)
    {
        _balance.enableNode(dockerID);
    }
    if (config.empty())
    {
        LOGE("at least have one docker contain ServiceUser service ");
        return false;
    }
    _nextUserID = ServerConfig::getRef().getAreaID() * (ui64)pow(10, 8);

    std::string sql = trim(UserPreview().getDBSelectPure(), " ");
    sql = subStringWithoutBack(sql, " ");
    sql += " `tb_UserBaseInfo` ";
    int curLimit = 0;
    SQLQueryReq req(sql + "limit 0, 100");
    toService(ServiceInfoDBMgr, req,
        std::bind(&UserMgrService::onInitUserPreviewsFromDB, std::static_pointer_cast<UserMgrService>(shared_from_this()), _1, curLimit, sql));
    return true;
}

void UserMgrService::onInitUserPreviewsFromDB(zsummer::proto4z::ReadStream & rs, int curLimit, const std::string &sql)
{
    SQLQueryResp resp;
    rs >> resp;
    if (resp.retCode != EC_SUCCESS)
    {
        LOGE("onInitUserPreviewsFromDB error. errCode=" << resp.retCode 
            << ", curLimit=" << curLimit << ",  inited user=" << _userStatusByID.size() <<  ", sql=" << sql);
        return;
    }
    if (resp.result.qc != QEC_SUCCESS )
    {
        LOGE("onInitUserPreviewsFromDB error. errCode=" << resp.retCode << ", resp.result.qc=" << resp.result.qc
            << ", sql msg=" << resp.result.errMsg
            << ", curLimit=" << curLimit << ",  inited user=" << _userStatusByID.size() << ", sql=" << sql);
        return;
    }
    if (resp.result.fields.empty())
    {
        LOGA("onInitUserPreviewsFromDB success. errCode=" << resp.retCode << ", resp.result.qc=" << resp.result.qc
            << ", sql msg=" << resp.result.errMsg
            << ", curLimit=" << curLimit << ",  inited user=" << _userStatusByID.size() << ", sql=" << sql);

        LOGD("onInitLastUIDFromDB _nextUserID=" << _nextUserID << ", areaID=" << ServerConfig::getRef().getAreaID()
            << ", area begin uid=" << ServerConfig::getRef().getAreaID() * (ui64)pow(10, 8));
        finishInit();
        return;
    }
    DBResult result;
    result.buildResult((QueryErrorCode)resp.result.qc, resp.result.errMsg, resp.result.sql, resp.result.affected, resp.result.fields);
    while (result.haveRow())
    {
        UserPreview up;
        up.fetchFromDBResult(result);

        if (_userStatusByID.find(up.serviceID) != _userStatusByID.end()
            || _userStatusByName.find(up.serviceName) != _userStatusByName.end())
        {
            LOGA("onInitUserPreviewsFromDB . errCode=" << resp.retCode << ", resp.result.qc=" << resp.result.qc
                << ", sql msg=" << resp.result.errMsg
                << ", curLimit=" << curLimit << ",  inited user=" << _userStatusByID.size() << ", sql=" << sql);
            LOGE("User ID or Name conflict. " << up);
            return;
        }
        updateUserPreview(up);
        if (_nextUserID < up.serviceID)
        {
            _nextUserID = up.serviceID;
        }
    }
    SQLQueryReq req(sql + "limit " + toString(curLimit+100) + ", 100");
    toService(ServiceInfoDBMgr, req,
        std::bind(&UserMgrService::onInitUserPreviewsFromDB, std::static_pointer_cast<UserMgrService>(shared_from_this()), _1, curLimit+100, sql));

}




void UserMgrService::updateUserPreview(const UserPreview & pre)
{
    UserStatusPtr usp;
    if (true)
    {
        auto founder = _userStatusByID.find(pre.serviceID);
        if (founder == _userStatusByID.end())
        {
            usp = std::make_shared<UserStatus>();
            usp->_status = SS_NONE;
            usp->_preview = pre;
            _userStatusByID[pre.serviceID] = usp;
            _userStatusByName[pre.serviceName] = usp;
        }
        else
        {
            usp = founder->second;
            usp->_preview = pre;
        }
        _userStatusByName[pre.serviceName] = usp;
    }
    if (true)
    {
        auto &acs = _accountStatus[pre.account];
        acs._users[pre.serviceID] = usp;
    }
}

void UserMgrService::onCreateOrRefreshServiceNotice(const Tracing & trace, zsummer::proto4z::ReadStream &rs)
{
    CreateOrRefreshServiceNotice notice;
    rs >> notice;
    if (notice.serviceType == ServiceUser)
    {
        auto founder = _userStatusByID.find(notice.serviceID);
        if (founder == _userStatusByID.end())
        {
            LOGE("error");
            return;
        }
        founder->second->_status = notice.status;
        founder->second->_lastChangeTime = getNowTime();
    }
}




void UserMgrService::onSelectUserPreviewsFromUserMgrReq(const Tracing & trace, zsummer::proto4z::ReadStream &rs)
{
    SelectUserPreviewsFromUserMgrReq req;
    rs >> req;

    if (req.account.empty() || req.token != req.token)
    {
        SelectUserPreviewsFromUserMgrResp resp;
        resp.account = req.account;
        resp.token = req.token;
        resp.previews.clear();
        resp.clientDockerID = req.clientDockerID;
        resp.clientSessionID = req.clientSessionID;
        resp.retCode = EC_PERMISSION_DENIED;
        Docker::getRef().sendToDocker(req.clientDockerID, resp); //这个是认证协议, 对应的UserService并不存在 所以不能通过toService和backToService等接口发出去.
    }
    else
    {
        std::string sql = trim(UserPreview().getDBSelectPure(), " ");
        sql = subStringWithoutBack(sql, " ");
        sql += " `tb_UserBaseInfo` where account=?;";
 
        DBQuery q(sql);
        q << req.account;
        SQLQueryReq sqlReq(q.pickSQL());
        toService(ServiceInfoDBMgr, sqlReq,
            std::bind(&UserMgrService::onSelectUserPreviewsFromUserMgrReqFromDB, std::static_pointer_cast<UserMgrService>(shared_from_this()), _1, trace, req));
    }
}
void UserMgrService::onSelectUserPreviewsFromUserMgrReqFromDB(zsummer::proto4z::ReadStream & rs, const Tracing & trace, const SelectUserPreviewsFromUserMgrReq & req)
{
    LOGD("UserMgrService::onSelectUserPreviewsFromUserMgrReqFromDB");
    DBResult dbResult;
    SQLQueryResp sqlResp;
    rs >> sqlResp;
    dbResult.buildResult((QueryErrorCode)sqlResp.result.qc, sqlResp.result.errMsg, sqlResp.result.sql, sqlResp.result.affected, sqlResp.result.fields);


    SelectUserPreviewsFromUserMgrResp resp;
    resp.account = req.account;
    resp.token = req.token;
    resp.previews.clear();
    resp.clientDockerID = req.clientDockerID;
    resp.clientSessionID = req.clientSessionID;
    resp.retCode = sqlResp.retCode != EC_SUCCESS ? sqlResp.retCode : (dbResult.getErrorCode() != QEC_SUCCESS ? EC_DB_ERROR : EC_SUCCESS);
    if (sqlResp.retCode == EC_SUCCESS && dbResult.getErrorCode() == QEC_SUCCESS )
    {
        _accountStatus[resp.account];
        while (dbResult.haveRow())
        {
            UserPreview pre;
            pre.fetchFromDBResult(dbResult);
            resp.previews.push_back(pre);
            updateUserPreview(pre);
        }
    }
    Docker::getRef().sendToDocker(req.clientDockerID, resp); //这个是认证协议, 对应的UserService并不存在 所以不能通过toService和backToService等接口发出去.
}

void UserMgrService::onCreateUserFromUserMgrReq(const Tracing & trace, zsummer::proto4z::ReadStream &rs)
{
    CreateUserFromUserMgrReq req;
    rs >> req;
    CreateUserFromUserMgrResp resp;
    resp.retCode = EC_SUCCESS;
    resp.previews.clear();
    resp.clientDockerID = req.clientDockerID;
    resp.clientSessionID = req.clientSessionID;

    auto founder = _accountStatus.find(req.account);
    if (founder == _accountStatus.end() || founder->second._users.size() > 4 || getNowTime() - founder->second._lastCreateTime < 60)
    {
        resp.retCode = EC_ERROR;
        Docker::getRef().sendToDocker(req.clientDockerID, resp); //这个是认证协议, 对应的UserService并不存在 所以不能通过toService和backToService等接口发出去.
        return;
    }

    if (_userStatusByName.find(req.serviceName) != _userStatusByName.end())
    {
        resp.retCode = EC_ERROR;
        Docker::getRef().sendToDocker(req.clientDockerID, resp); //这个是认证协议, 对应的UserService并不存在 所以不能通过toService和backToService等接口发出去.
        return;
    }
    founder->second._lastCreateTime = getNowTime();
    UserBaseInfo userBaseInfo;
    userBaseInfo.account = req.account;
    userBaseInfo.serviceID = ++_nextUserID;
    userBaseInfo.serviceName = req.serviceName;
    userBaseInfo.level = 1;
    userBaseInfo.iconID = 0;

    SQLQueryReq sql(userBaseInfo.getDBInsert());
    toService(ServiceInfoDBMgr, sql,
        std::bind(&UserMgrService::onCreateUserFromUserMgrReqFromDB, std::static_pointer_cast<UserMgrService>(shared_from_this()), _1, userBaseInfo, req));
}

void UserMgrService::onCreateUserFromUserMgrReqFromDB(zsummer::proto4z::ReadStream & rs, const UserBaseInfo & ubi, const CreateUserFromUserMgrReq &req)
{
    LOGD("UserMgrService::onCreateUserFromUserMgrReqFromDB");
    SQLQueryResp sqlResp;
    rs >> sqlResp;

    CreateUserFromUserMgrResp resp;
    resp.retCode = EC_SUCCESS;
    resp.previews.clear();
    resp.clientDockerID = req.clientDockerID;
    resp.clientSessionID = req.clientSessionID;
    resp.retCode = sqlResp.retCode;
    
    if (sqlResp.result.qc != QEC_SUCCESS || sqlResp.result.affected == 0)
    {
        resp.retCode = EC_DB_ERROR;
    }
    if (resp.retCode == EC_SUCCESS)
    {
        UserPreview up(ubi.serviceID, ubi.serviceName, ubi.account, ubi.iconID, ubi.level);
        updateUserPreview(up);
        for (const auto & kv : _accountStatus[ubi.account]._users)
        {
            resp.previews.push_back(kv.second->_preview);
        }
    }

    Docker::getRef().sendToDocker(req.clientDockerID, resp); //这个是认证协议, 对应的UserService并不存在 所以不能通过toService和backToService等接口发出去.
}

void UserMgrService::onAttachUserFromUserMgrReq(const Tracing & trace, zsummer::proto4z::ReadStream &rs)
{
    AttachUserFromUserMgrReq req;
    rs >> req;
    AttachUserFromUserMgrResp resp;
    resp.retCode = EC_SUCCESS;
    resp.serviceID = req.serviceID;
    resp.clientDockerID = req.clientDockerID;
    resp.clientSessionID = req.clientSessionID;

    auto founder = _userStatusByID.find(req.serviceID);
    if (founder == _userStatusByID.end() || founder->second->_preview.account != req.account)
    {
        resp.retCode = EC_ERROR;
        Docker::getRef().sendToDocker(req.clientDockerID, resp); //这个是认证协议, 对应的UserService并不存在 所以不能通过toService和backToService等接口发出去.
        return;
    }
    auto & status = *founder->second;
    if (status._status == SS_UNINITING || status._status == SS_INITING )
    {
        resp.retCode = EC_ERROR;
        Docker::getRef().sendToDocker(req.clientDockerID, resp); //这个是认证协议, 对应的UserService并不存在 所以不能通过toService和backToService等接口发出去.
        return;
    }
    status._lastChangeTime = getNowTime();
    _freeList.erase(req.serviceID);
    if (status._status == SS_NONE || status._status == SS_DESTROY)
    {
        DockerID dockerID = _balance.selectAuto();
        if (dockerID == InvalidDockerID)
        {
            resp.retCode = EC_ERROR;
            Docker::getRef().sendToDocker(req.clientDockerID, resp); //这个是认证协议, 对应的UserService并不存在 所以不能通过toService和backToService等接口发出去.
            return;
        }
        status._status = SS_INITING;
        status._clientDockerID = req.clientDockerID;
        status._clientSessionID = req.clientSessionID;
        CreateServiceInDocker notice(ServiceUser, req.serviceID, status._preview.serviceName, req.clientDockerID, req.clientSessionID);
        Docker::getRef().sendToDocker(dockerID, notice);
    }
    else if(status._status == SS_WORKING)
    {
        status._clientDockerID = req.clientDockerID;
        status._clientSessionID = req.clientSessionID;
        ChangeServiceClient change(ServiceUser, req.serviceID, req.clientDockerID, req.clientSessionID);
        Docker::getRef().sendToDockerByService(ServiceUser, req.serviceID, change);
    }
    else
    {
        resp.retCode = EC_ERROR; 
        Docker::getRef().sendToDocker(req.clientDockerID, resp); //这个是认证协议, 对应的UserService并不存在 所以不能通过toService和backToService等接口发出去.
        return;
    }
}


void UserMgrService::onRealClientClosedNotice(const Tracing & trace, zsummer::proto4z::ReadStream & rs)
{
    RealClientClosedNotice notice;
    rs >> notice;
    auto founder = _userStatusByID.find(notice.serviceID);
    if (founder == _userStatusByID.end())
    {
        LOGE("onRealClientClosedNotice not found service id.  " << notice.serviceID);
        return;
    }
    if (founder->second->_clientDockerID == notice.clientDockerID
        && founder->second->_clientSessionID == notice.clientSessionID
        && founder->second->_status == SS_WORKING)
    {
        ChangeServiceClient change(ServiceUser, notice.serviceID, InvalidDockerID, InvalidSessionID);
        Docker::getRef().sendToDockerByService(ServiceUser, notice.serviceID, change);
        founder->second->_lastChangeTime = getNowTime();
        _freeList[notice.serviceID] = founder->second;
    }

}



