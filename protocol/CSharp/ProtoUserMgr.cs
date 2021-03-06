 
namespace Proto4z  
{ 
 
    public class SelectUserPreviewsFromUserMgrReq: Proto4z.IProtoObject //获取角色预览数据  
    {     
        //proto id   
        public const ushort protoID = 4000;  
        static public ushort getProtoID() { return 4000; } 
        static public string getProtoName() { return "SelectUserPreviewsFromUserMgrReq"; } 
        //members   
        public string account;  
        public string token;  
        public uint clientDockerID;  
        public uint clientSessionID;  
        public SelectUserPreviewsFromUserMgrReq()  
        { 
            account = "";  
            token = "";  
            clientDockerID = 0;  
            clientSessionID = 0;  
        } 
        public SelectUserPreviewsFromUserMgrReq(string account, string token, uint clientDockerID, uint clientSessionID) 
        { 
            this.account = account; 
            this.token = token; 
            this.clientDockerID = clientDockerID; 
            this.clientSessionID = clientSessionID; 
        } 
        public System.Collections.Generic.List<byte> __encode() 
        { 
            var data = new System.Collections.Generic.List<byte>(); 
            data.AddRange(Proto4z.BaseProtoObject.encodeString(this.account)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeString(this.token)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI32(this.clientDockerID)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI32(this.clientSessionID)); 
            return data; 
        } 
        public int __decode(byte[] binData, ref int pos) 
        { 
            this.account = Proto4z.BaseProtoObject.decodeString(binData, ref pos); 
            this.token = Proto4z.BaseProtoObject.decodeString(binData, ref pos); 
            this.clientDockerID = Proto4z.BaseProtoObject.decodeUI32(binData, ref pos); 
            this.clientSessionID = Proto4z.BaseProtoObject.decodeUI32(binData, ref pos); 
            return pos; 
        } 
    } 
 
    public class SelectUserPreviewsFromUserMgrResp: Proto4z.IProtoObject //获取角色预览数据  
    {     
        //proto id   
        public const ushort protoID = 4001;  
        static public ushort getProtoID() { return 4001; } 
        static public string getProtoName() { return "SelectUserPreviewsFromUserMgrResp"; } 
        //members   
        public ushort retCode;  
        public string account;  
        public string token;  
        public uint clientDockerID;  
        public uint clientSessionID;  
        public UserPreviewArray previews;  
        public SelectUserPreviewsFromUserMgrResp()  
        { 
            retCode = 0;  
            account = "";  
            token = "";  
            clientDockerID = 0;  
            clientSessionID = 0;  
            previews = new UserPreviewArray();  
        } 
        public SelectUserPreviewsFromUserMgrResp(ushort retCode, string account, string token, uint clientDockerID, uint clientSessionID, UserPreviewArray previews) 
        { 
            this.retCode = retCode; 
            this.account = account; 
            this.token = token; 
            this.clientDockerID = clientDockerID; 
            this.clientSessionID = clientSessionID; 
            this.previews = previews; 
        } 
        public System.Collections.Generic.List<byte> __encode() 
        { 
            var data = new System.Collections.Generic.List<byte>(); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI16(this.retCode)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeString(this.account)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeString(this.token)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI32(this.clientDockerID)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI32(this.clientSessionID)); 
            if (this.previews == null) this.previews = new UserPreviewArray(); 
            data.AddRange(this.previews.__encode()); 
            return data; 
        } 
        public int __decode(byte[] binData, ref int pos) 
        { 
            this.retCode = Proto4z.BaseProtoObject.decodeUI16(binData, ref pos); 
            this.account = Proto4z.BaseProtoObject.decodeString(binData, ref pos); 
            this.token = Proto4z.BaseProtoObject.decodeString(binData, ref pos); 
            this.clientDockerID = Proto4z.BaseProtoObject.decodeUI32(binData, ref pos); 
            this.clientSessionID = Proto4z.BaseProtoObject.decodeUI32(binData, ref pos); 
            this.previews = new UserPreviewArray(); 
            this.previews.__decode(binData, ref pos); 
            return pos; 
        } 
    } 
 
    public class CreateUserFromUserMgrReq: Proto4z.IProtoObject //创角色请求  
    {     
        //proto id   
        public const ushort protoID = 4002;  
        static public ushort getProtoID() { return 4002; } 
        static public string getProtoName() { return "CreateUserFromUserMgrReq"; } 
        //members   
        public string account;  
        public string serviceName;  
        public uint clientDockerID;  
        public uint clientSessionID;  
        public CreateUserFromUserMgrReq()  
        { 
            account = "";  
            serviceName = "";  
            clientDockerID = 0;  
            clientSessionID = 0;  
        } 
        public CreateUserFromUserMgrReq(string account, string serviceName, uint clientDockerID, uint clientSessionID) 
        { 
            this.account = account; 
            this.serviceName = serviceName; 
            this.clientDockerID = clientDockerID; 
            this.clientSessionID = clientSessionID; 
        } 
        public System.Collections.Generic.List<byte> __encode() 
        { 
            var data = new System.Collections.Generic.List<byte>(); 
            data.AddRange(Proto4z.BaseProtoObject.encodeString(this.account)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeString(this.serviceName)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI32(this.clientDockerID)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI32(this.clientSessionID)); 
            return data; 
        } 
        public int __decode(byte[] binData, ref int pos) 
        { 
            this.account = Proto4z.BaseProtoObject.decodeString(binData, ref pos); 
            this.serviceName = Proto4z.BaseProtoObject.decodeString(binData, ref pos); 
            this.clientDockerID = Proto4z.BaseProtoObject.decodeUI32(binData, ref pos); 
            this.clientSessionID = Proto4z.BaseProtoObject.decodeUI32(binData, ref pos); 
            return pos; 
        } 
    } 
 
    public class CreateUserFromUserMgrResp: Proto4z.IProtoObject //创角色请求返回  
    {     
        //proto id   
        public const ushort protoID = 4003;  
        static public ushort getProtoID() { return 4003; } 
        static public string getProtoName() { return "CreateUserFromUserMgrResp"; } 
        //members   
        public ushort retCode;  
        public uint clientDockerID;  
        public uint clientSessionID;  
        public ulong serviceID;  
        public UserPreviewArray previews;  
        public CreateUserFromUserMgrResp()  
        { 
            retCode = 0;  
            clientDockerID = 0;  
            clientSessionID = 0;  
            serviceID = 0;  
            previews = new UserPreviewArray();  
        } 
        public CreateUserFromUserMgrResp(ushort retCode, uint clientDockerID, uint clientSessionID, ulong serviceID, UserPreviewArray previews) 
        { 
            this.retCode = retCode; 
            this.clientDockerID = clientDockerID; 
            this.clientSessionID = clientSessionID; 
            this.serviceID = serviceID; 
            this.previews = previews; 
        } 
        public System.Collections.Generic.List<byte> __encode() 
        { 
            var data = new System.Collections.Generic.List<byte>(); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI16(this.retCode)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI32(this.clientDockerID)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI32(this.clientSessionID)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI64(this.serviceID)); 
            if (this.previews == null) this.previews = new UserPreviewArray(); 
            data.AddRange(this.previews.__encode()); 
            return data; 
        } 
        public int __decode(byte[] binData, ref int pos) 
        { 
            this.retCode = Proto4z.BaseProtoObject.decodeUI16(binData, ref pos); 
            this.clientDockerID = Proto4z.BaseProtoObject.decodeUI32(binData, ref pos); 
            this.clientSessionID = Proto4z.BaseProtoObject.decodeUI32(binData, ref pos); 
            this.serviceID = Proto4z.BaseProtoObject.decodeUI64(binData, ref pos); 
            this.previews = new UserPreviewArray(); 
            this.previews.__decode(binData, ref pos); 
            return pos; 
        } 
    } 
 
    public class AttachUserFromUserMgrReq: Proto4z.IProtoObject //选角色请求  
    {     
        //proto id   
        public const ushort protoID = 4004;  
        static public ushort getProtoID() { return 4004; } 
        static public string getProtoName() { return "AttachUserFromUserMgrReq"; } 
        //members   
        public ulong serviceID;  
        public string account;  
        public uint clientDockerID;  
        public uint clientSessionID;  
        public AttachUserFromUserMgrReq()  
        { 
            serviceID = 0;  
            account = "";  
            clientDockerID = 0;  
            clientSessionID = 0;  
        } 
        public AttachUserFromUserMgrReq(ulong serviceID, string account, uint clientDockerID, uint clientSessionID) 
        { 
            this.serviceID = serviceID; 
            this.account = account; 
            this.clientDockerID = clientDockerID; 
            this.clientSessionID = clientSessionID; 
        } 
        public System.Collections.Generic.List<byte> __encode() 
        { 
            var data = new System.Collections.Generic.List<byte>(); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI64(this.serviceID)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeString(this.account)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI32(this.clientDockerID)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI32(this.clientSessionID)); 
            return data; 
        } 
        public int __decode(byte[] binData, ref int pos) 
        { 
            this.serviceID = Proto4z.BaseProtoObject.decodeUI64(binData, ref pos); 
            this.account = Proto4z.BaseProtoObject.decodeString(binData, ref pos); 
            this.clientDockerID = Proto4z.BaseProtoObject.decodeUI32(binData, ref pos); 
            this.clientSessionID = Proto4z.BaseProtoObject.decodeUI32(binData, ref pos); 
            return pos; 
        } 
    } 
 
    public class AttachUserFromUserMgrResp: Proto4z.IProtoObject //选角色请求返回  
    {     
        //proto id   
        public const ushort protoID = 4005;  
        static public ushort getProtoID() { return 4005; } 
        static public string getProtoName() { return "AttachUserFromUserMgrResp"; } 
        //members   
        public ushort retCode;  
        public uint clientDockerID;  
        public uint clientSessionID;  
        public ulong serviceID;  
        public AttachUserFromUserMgrResp()  
        { 
            retCode = 0;  
            clientDockerID = 0;  
            clientSessionID = 0;  
            serviceID = 0;  
        } 
        public AttachUserFromUserMgrResp(ushort retCode, uint clientDockerID, uint clientSessionID, ulong serviceID) 
        { 
            this.retCode = retCode; 
            this.clientDockerID = clientDockerID; 
            this.clientSessionID = clientSessionID; 
            this.serviceID = serviceID; 
        } 
        public System.Collections.Generic.List<byte> __encode() 
        { 
            var data = new System.Collections.Generic.List<byte>(); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI16(this.retCode)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI32(this.clientDockerID)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI32(this.clientSessionID)); 
            data.AddRange(Proto4z.BaseProtoObject.encodeUI64(this.serviceID)); 
            return data; 
        } 
        public int __decode(byte[] binData, ref int pos) 
        { 
            this.retCode = Proto4z.BaseProtoObject.decodeUI16(binData, ref pos); 
            this.clientDockerID = Proto4z.BaseProtoObject.decodeUI32(binData, ref pos); 
            this.clientSessionID = Proto4z.BaseProtoObject.decodeUI32(binData, ref pos); 
            this.serviceID = Proto4z.BaseProtoObject.decodeUI64(binData, ref pos); 
            return pos; 
        } 
    } 
 
} 
 
 
