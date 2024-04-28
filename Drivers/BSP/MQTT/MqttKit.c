/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	MqttKit.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2018-04-27
	*
	*	�汾�� 		V1.6
	*
	*	˵���� 		MQTTЭ��
	*
	*	�޸ļ�¼��	V1.1�����MQTT_PacketSubscribe���Ĳ�Ϊ2��topic
	*						����ʱЭ������bug
	*				V1.2���޸�MQTT_PacketCmdResp��bug
	*				V1.3����strncpy�滻Ϊmemcpy�����Ǳ��bug
	*				V1.4���޸�	MQTT_PacketPublishAck
	*							MQTT_PacketPublishRel
	*							������������bug
	*				V1.5������	MQTT_UnPacketCmd
	*							MQTT_UnPacketPublish
	*							�ӿڶ���Ϣ���ݳ��ȵ���ȡ����
	*				V1.6�����Ӷ������ļ��ϴ��ӿ�
	************************************************************
	************************************************************
	************************************************************
**/

//Э��ͷ�ļ�
#include "./BSP/MQTT/MqttKit.h"
#include "./SYSTEM/sys/sys.h"

//C��
#include <string.h>
#include <stdio.h>


#define CMD_TOPIC_PREFIX		"$creq"



//==========================================================
//	�������ƣ�	EDP_NewBuffer
//
//	�������ܣ�	�����ڴ�
//
//	��ڲ�����	edpPacket�����ṹ��
//				size����С
//
//	���ز�����	��
//
//	˵����		1.��ʹ�ö�̬�����������ڴ�
//				2.��ʹ�þֲ���ȫ��������ָ���ڴ�
//==========================================================
void MQTT_NewBuffer(MQTT_PACKET_STRUCTURE *mqttPacket, uint32_t size)
{

    char  MQTT_pack_Buffer[size];

    	uint32_t i = 0;

	if(mqttPacket->_data == NULL)
	{
		mqttPacket->_memFlag = 2;
		
		//mqttPacket->_data = (char *)MQTT_MallocBuffer(size);
            mqttPacket->_data = MQTT_pack_Buffer;
		if(mqttPacket->_data != NULL)
		{
			mqttPacket->_len = 0;
			
			mqttPacket->_size = size;
			
			for(; i < mqttPacket->_size; i++)
				mqttPacket->_data[i] = 0;
		}
	}
        else
        {
            mqttPacket->_memFlag = MEM_FLAG_STATIC;
            
            for(; i < mqttPacket->_size; i++)
                mqttPacket->_data[i] = 0;
            
                mqttPacket->_len = 0;
            
            if(mqttPacket->_size < size)
                mqttPacket->_data = NULL;
        }


//	uint32_t i = 0;

//	if(mqttPacket->_data == NULL)
//	{
//		mqttPacket->_memFlag = MEM_FLAG_ALLOC;
//		
//		mqttPacket->_data = (char *)MQTT_MallocBuffer(size);
//		if(mqttPacket->_data != NULL)
//		{
//			mqttPacket->_len = 0;
//			
//			mqttPacket->_size = size;
//			
//			for(; i < mqttPacket->_size; i++)
//				mqttPacket->_data[i] = 0;
//		}
//	}
//        else
//        {
//		mqttPacket->_memFlag = MEM_FLAG_STATIC;
//		
//		for(; i < mqttPacket->_size; i++)
//			mqttPacket->_data[i] = 0;
//		
//		mqttPacket->_len = 0;
//		
//		if(mqttPacket->_size < size)
//			mqttPacket->_data = NULL;
//        }

}

//==========================================================
//	�������ƣ�	MQTT_DeleteBuffer
//
//	�������ܣ�	�ͷ������ڴ�
//
//	��ڲ�����	edpPacket�����ṹ��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void MQTT_DeleteBuffer(MQTT_PACKET_STRUCTURE *mqttPacket)
{

	if(mqttPacket->_memFlag == MEM_FLAG_ALLOC)
		MQTT_FreeBuffer(mqttPacket->_data);
	
	mqttPacket->_data = NULL;
	mqttPacket->_len = 0;
	mqttPacket->_size = 0;
	mqttPacket->_memFlag = MEM_FLAG_NULL;

}

int32_t MQTT_DumpLength(uint32_t len, uint8_t *buf)
{
	
	int32_t i = 0;
	
	for(i = 1; i <= 4; ++i)
	{
		*buf = len % 128;
		len >>= 7;
		if(len > 0)
		{
			*buf |= 128;
			++buf;
		}
		else
		{
			return i;
		}
	}

	return -1;
}

int32_t MQTT_ReadLength(const uint8_t *stream, int32_t size, uint32_t *len)//2,4,0
{
	
	int32_t i;
	const uint8_t *in = stream;//��2���ֽڴ��ʣ����Ϣ����
	uint32_t multiplier = 1;

	*len = 0;
	for(i = 0; i < size; ++i)
	{
		*len += (in[i] & 0x7f) * multiplier;

		if(!(in[i] & 0x80))//��2���ֽڵڰ�λ��0 �粻�����ǳ��ִ���
		{
			return i + 1;
		}

		multiplier <<= 7;
		if(multiplier >= 2097152)		//128 * *128 * *128
		{
			return -2;					// error, out of range �ڴ�ָ�����
		}
	}

	return -1;							// not complete

}

//==========================================================
//	�������ƣ�	MQTT_UnPacketRecv
//
//	�������ܣ�	MQTT���ݽ��������ж�
//
//	��ڲ�����	dataPtr�����յ�����ָ��
//
//	���ز�����	0-�ɹ�		����-ʧ��ԭ��
//
//	˵����		
//==========================================================
uint8_t MQTT_UnPacketRecv(uint8_t *dataPtr)
{
	
	uint8_t status = 255;
	uint8_t type = dataPtr[0] >> 4;				//���ͼ��
	
	if(type < 1 || type > 14)
		return status;
	
	if(type == MQTT_PKT_PUBLISH)
	{

		uint8_t *msgPtr;
		uint32_t remain_len = 0;

		msgPtr = dataPtr + MQTT_ReadLength(dataPtr + 1, 4, &remain_len) + 1;  //�����̶�ͷ���忴MQTTЭ��

		if(remain_len < 2 || dataPtr[0] & 0x01)					//retain
			return 255;
		
		if(remain_len < ((uint16_t)msgPtr[0] << 8 | msgPtr[1]) + 2)
			return 255;
		
		if(strstr((const char *)msgPtr + 2, CMD_TOPIC_PREFIX) != NULL)	//����������·�
			status = MQTT_PKT_CMD;
		else
			status = MQTT_PKT_PUBLISH;
	}
	else
		status = type;
	
	return status;

}

//==========================================================
//	�������ƣ�	MQTT_PacketConnect
//
//	�������ܣ�	������Ϣ���
//
//	��ڲ�����	user���û�������ƷID
//				password�����룺��Ȩ��Ϣ��apikey
//				devid���豸ID
//				cTime�����ӱ���ʱ��
//				clean_session��������Ϣ�����־
//				qos���ط���־
//				will_topic���쳣����topic
//				will_msg���쳣������Ϣ
//				will_retain����Ϣ���ͱ�־
//				mqttPacket����ָ��
//
//	���ز�����	0-�ɹ�		����-ʧ��
//
//	˵����		
//==========================================================
uint8_t MQTT_PacketConnect(const char *user, const char *password, const char *devid,
						uint16_t cTime, uint8_t clean_session, uint8_t qos,
						const char *will_topic, const char *will_msg, int32_t will_retain,
						MQTT_PACKET_STRUCTURE *mqttPacket)
{
	
	uint8_t flags = 0;
	uint8_t will_topic_len = 0;
	uint16_t total_len = 15;
	int16_t len = 0, devid_len = strlen((char*)devid);
	
	if(!devid)
		return 1;
	
	total_len += devid_len + 2;
	
	//���ߺ��Ƿ�����������Ϣ��1-����	0-������--------------------------------------------
	if(clean_session)
	{
		flags |= MQTT_CONNECT_CLEAN_SESSION;
	}
	
	//�쳣��������£�������������topic------------------------------------------------------
	if(will_topic)
	{
		flags |= MQTT_CONNECT_WILL_FLAG;
		will_topic_len = strlen((char*)will_topic);
		total_len += 4 + will_topic_len + strlen((char*)will_msg);
	}
	
	//qos����--��Ҫ����PUBLISH������̬����Ϣ�ģ���֤��Ϣ���ݵĴ���-----------------------------
	switch((unsigned char)qos)
	{
		case MQTT_QOS_LEVEL0:
			flags |= MQTT_CONNECT_WILL_QOS0;							//���һ��
		break;
		
		case MQTT_QOS_LEVEL1:
			flags |= (MQTT_CONNECT_WILL_FLAG | MQTT_CONNECT_WILL_QOS1);	//����һ��
		break;
		
		case MQTT_QOS_LEVEL2:
			flags |= (MQTT_CONNECT_WILL_FLAG | MQTT_CONNECT_WILL_QOS2);	//ֻ��һ��
		break;
		
		default:
		return 2;
	}
	
	//��Ҫ����PUBLISH(����̬)����Ϣ����ʾ������Ҫ����������͵���Ϣ��������µĶ����߳��֣��Ͱ�����Ϣ���͸��������������ô��������ǰ���ĵľ��ͷ���
	if(will_retain)
	{
		flags |= (MQTT_CONNECT_WILL_FLAG | MQTT_CONNECT_WILL_RETAIN);
	}
	
	//�˺�Ϊ�� ����Ϊ��---------------------------------------------------------------------
	if(!user || !password)
	{
		return 3;
	}
	flags |= MQTT_CONNECT_USER_NAME | MQTT_CONNECT_PASSORD;
	
	total_len += strlen((char*)user) + strlen((char*)password) + 4;
	
	//�����ڴ�-----------------------------------------------------------------------------
	MQTT_NewBuffer(mqttPacket, total_len);
	if(mqttPacket->_data == NULL)
		return 4;
	
	memset(mqttPacket->_data, 0, total_len);
	
/*************************************�̶�ͷ��***********************************************/
	
	//�̶�ͷ��----------------------������������---------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_CONNECT << 4;
	
	//�̶�ͷ��----------------------ʣ�೤��ֵ-----------------------------------------------
	len = MQTT_DumpLength(total_len - 5, (uint8_t*)mqttPacket->_data + mqttPacket->_len);
	if(len < 0)
	{
		MQTT_DeleteBuffer(mqttPacket);
		return 5;
	}
	else
		mqttPacket->_len += len;
	
/*************************************�ɱ�ͷ��***********************************************/
	
	//�ɱ�ͷ��----------------------Э�������� �� Э����--------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = 0;
	mqttPacket->_data[mqttPacket->_len++] = 4;
	mqttPacket->_data[mqttPacket->_len++] = 'M';
	mqttPacket->_data[mqttPacket->_len++] = 'Q';
	mqttPacket->_data[mqttPacket->_len++] = 'T';
	mqttPacket->_data[mqttPacket->_len++] = 'T';
	
	//�ɱ�ͷ��----------------------protocol level 4-----------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = 4;
	
	//�ɱ�ͷ��----------------------���ӱ�־(�ú�����ͷ����������)-----------------------------
    mqttPacket->_data[mqttPacket->_len++] = flags;
	
	//�ɱ�ͷ��----------------------�������ӵ�ʱ��(��)----------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(cTime);
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(cTime);
	 
/*************************************��Ϣ��************************************************/

	//��Ϣ��----------------------------devid���ȡ�devid-------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(devid_len);
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(devid_len);
	
	strncat((char *)mqttPacket->_data + mqttPacket->_len, (char *)devid, devid_len);
	mqttPacket->_len += devid_len;
	
	//��Ϣ��----------------------------will_flag �� will_msg---------------------------------
	if(flags & MQTT_CONNECT_WILL_FLAG)
	{
		unsigned short mLen = 0;
		
		if(!will_msg)
			will_msg = "";
		
		mLen = strlen((char *)will_topic);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(mLen);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(mLen);
		strncat((char *)mqttPacket->_data + mqttPacket->_len, (char *)will_topic, mLen);
		mqttPacket->_len += mLen;
		
		mLen = strlen(will_msg);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(mLen);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(mLen);
		strncat((char *)mqttPacket->_data + mqttPacket->_len, will_msg, mLen);
		mqttPacket->_len += mLen;
	}
	
	//��Ϣ��----------------------------use---------------------------------------------------
	if(flags & MQTT_CONNECT_USER_NAME)
	{
		unsigned short user_len = strlen((char *)user);
		
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(user_len);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(user_len);
		strncat((char *)mqttPacket->_data + mqttPacket->_len, (char *)user, user_len);
		mqttPacket->_len += user_len;
	}

	//��Ϣ��----------------------------password----------------------------------------------
	if(flags & MQTT_CONNECT_PASSORD)
	{
		unsigned short psw_len = strlen((char *)password);
		
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(psw_len);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(psw_len);
		strncat((char *)mqttPacket->_data + mqttPacket->_len, (char *)password, psw_len);
		mqttPacket->_len += psw_len;
	}

	return 0;

}

//==========================================================
//	�������ƣ�	MQTT_PacketDisConnect
//
//	�������ܣ�	�Ͽ�������Ϣ���
//
//	��ڲ�����	mqttPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint8_t MQTT_PacketDisConnect(MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 2);
	if(mqttPacket->_data == NULL)
		return 1;
	
/*************************************�̶�ͷ��***********************************************/
	
	//�̶�ͷ��----------------------ͷ����Ϣ-------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_DISCONNECT << 4;
	
	//�̶�ͷ��----------------------ʣ�೤��ֵ-----------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = 0;
	
	return 0;

}

//==========================================================
//	�������ƣ�	MQTT_UnPacketConnectAck
//
//	�������ܣ�	������Ϣ���
//
//	��ڲ�����	rev_data�����յ�����
//
//	���ز�����	1��255-ʧ��		����-ƽ̨�ķ�����
//
//	˵����		
//==========================================================
uint8_t MQTT_UnPacketConnectAck(uint8_t *rev_data)
{

	if(rev_data[1] != 2)
		return 1;
	
	if(rev_data[2] == 0 || rev_data[2] == 1)
		return rev_data[3];
	else
		return 255;

}

//==========================================================
//	�������ƣ�	MQTT_PacketSaveData
//
//	�������ܣ�	���ݵ��ϴ����
//
//	��ڲ�����	devid���豸ID(��Ϊ��)
//				send_buf��json����buf
//				send_len��json�ܳ�
//				type_bin_head��bin�ļ�����Ϣͷ
//				type������
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint8_t MQTT_PacketSaveData(const int8_t *devid, int16_t send_len, int8_t *type_bin_head, uint8_t type, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, "$dp", NULL, send_len + 3, MQTT_QOS_LEVEL1, 0, 1, mqttPacket) == 0)
	{
		mqttPacket->_data[mqttPacket->_len++] = type;					//����
		
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(send_len);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(send_len);
	}
	else
		return 1;
	
	return 0;

}

//==========================================================
//	�������ƣ�	MQTT_PacketSaveBinData
//
//	�������ܣ�	Ϊ��ֹ�ļ��ϴ����
//
//	��ڲ�����	name������������
//				file_len���ļ�����
//				mqttPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint8_t MQTT_PacketSaveBinData(const int8_t *name, int16_t file_len, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	uint8_t result = 1;
	int8_t *bin_head = NULL;
	uint8_t bin_head_len = 0;
	char *payload = NULL;
	int32_t payload_size = 0;
	
	bin_head = (int8_t *)MQTT_MallocBuffer(13 + strlen((char *)name));
	if(bin_head == NULL)
		return result;
	
	sprintf((char *)bin_head, "{\"ds_id\":\"%s\"}", name);
	
	bin_head_len = strlen((char *)bin_head);
	payload_size = 7 + bin_head_len + file_len;
	
	payload = (char *)MQTT_MallocBuffer(payload_size - file_len);
	if(payload == NULL)
	{
		MQTT_FreeBuffer(bin_head);
		
		return result;
	}
	
	payload[0] = 2;						//����
		
	payload[1] = MOSQ_MSB(bin_head_len);
	payload[2] = MOSQ_LSB(bin_head_len);
	
	memcpy(payload + 3, bin_head, bin_head_len);
	
	payload[bin_head_len + 3] = (file_len >> 24) & 0xFF;
	payload[bin_head_len + 4] = (file_len >> 16) & 0xFF;
	payload[bin_head_len + 5] = (file_len >> 8) & 0xFF;
	payload[bin_head_len + 6] = file_len & 0xFF;
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, "$dp", payload, payload_size, MQTT_QOS_LEVEL1, 0, 1, mqttPacket) == 0)
		result = 0;
	
	MQTT_FreeBuffer(bin_head);
	MQTT_FreeBuffer(payload);
	
	return result;

}

//==========================================================
//	�������ƣ�	MQTT_UnPacketCmd
//
//	�������ܣ�	�����·����
//
//	��ڲ�����	rev_data�����յ�����ָ��
//				cmdid��cmdid-uuid
//				req������
//
//	���ز�����	0-�ɹ�		����-ʧ��ԭ��
//
//	˵����		
//==========================================================
uint8_t MQTT_UnPacketCmd(uint8_t *rev_data, char **cmdid, char **req, uint16_t *req_len)
{
printf( "Cmd1\r\n");
	char *dataPtr = strchr((char *)rev_data + 6, '/');	//��6������ͷ��Ϣ
	
	uint32_t remain_len = 0;
	
	if(dataPtr == NULL)									//δ�ҵ�'/'
		return 1;
	dataPtr++;											//����'/'
	
	MQTT_ReadLength(rev_data + 1, 4, &remain_len);		//��ȡʣ���ֽ�
	printf( "Cmd2\r\n");
	*cmdid = (char *)MQTT_MallocBuffer(37);				//cmdid�̶�36�ֽڣ������һ����������λ��
	if(*cmdid == NULL)
		return 2;
	
	memset(*cmdid, 0, 37);								//ȫ������
	memcpy(*cmdid, (const int8_t *)dataPtr, 36);			//����cmdid
	dataPtr += 36;
		printf( "Cmd3\r\n");
	*req_len = remain_len - 44;							//����� = ʣ�೤��(remain_len) - 2 - 5($creq) - 1(\) - cmdid����
	*req = (char *)MQTT_MallocBuffer(*req_len + 1);		//���������+1
    
    
    		printf( "Cmd4\r\n");
	if(*req == NULL)
	{
		MQTT_FreeBuffer(*cmdid);
		return 3;
	}
	
	memset(*req, 0, *req_len + 1);						//����
	memcpy(*req, (const int8_t *)dataPtr, *req_len);		//��������
	
	return 0;

}

//==========================================================
//	�������ƣ�	MQTT_PacketCmdResp
//
//	�������ܣ�	����ظ����
//
//	��ڲ�����	cmdid��cmdid
//				req������
//				mqttPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint8_t MQTT_PacketCmdResp(const int8_t *cmdid, const char *req, MQTT_PACKET_STRUCTURE *mqttPacket)
{
	
	uint16_t cmdid_len = strlen((char*)cmdid);
	uint16_t req_len = strlen((char*)req);
	_Bool status = 0;
	
	char *payload = MQTT_MallocBuffer(cmdid_len + 7);
	if(payload == NULL)
		return 1;
	
	memset(payload, 0, cmdid_len + 7);
	memcpy(payload, "$crsp/", 6);
	strncat((char*)payload, (char*)cmdid, cmdid_len);

	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, payload, req, strlen((char*)req), MQTT_QOS_LEVEL0, 0, 1, mqttPacket) == 0)
		status = 0;
	else
		status = 1;
	
	MQTT_FreeBuffer(payload);
	
	return status;

}

//==========================================================
//	�������ƣ�	MQTT_PacketSubscribe
//
//	�������ܣ�	Subscribe��Ϣ���
//
//	��ڲ�����	pkt_id��pkt_id
//				qos����Ϣ�ط�����
//				topics�����ĵ���Ϣ
//				topics_cnt�����ĵ���Ϣ����
//				mqttPacket����ָ��
//
//	���ز�����	0-�ɹ�		����-ʧ��
//
//	˵����		
//==========================================================
uint8_t MQTT_PacketSubscribe(uint16_t pkt_id, enum MqttQosLevel qos, const char *topics[], uint8_t topics_cnt, MQTT_PACKET_STRUCTURE *mqttPacket)
{
	
	uint32_t topic_len = 0, remain_len = 0;
	int16_t len = 0;
	uint8_t i = 0;
	
	if(pkt_id == 0)
		return 1;
	
	//����topic����-------------------------------------------------------------------------
	for(; i < topics_cnt; i++)
	{
		if(topics[i] == NULL)
			return 2;
		
		topic_len += strlen((char*)topics[i]);
	}
	
	//2 bytes packet id + topic filter(2 bytes topic + topic length + 1 byte reserve)------
	remain_len = 2 + 3 * topics_cnt + topic_len;
	
	//�����ڴ�------------------------------------------------------------------------------
	MQTT_NewBuffer(mqttPacket, remain_len + 5);
	if(mqttPacket->_data == NULL)
		return 3;
	
/*************************************�̶�ͷ��***********************************************/
	
	//�̶�ͷ��----------------------ͷ����Ϣ-------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_SUBSCRIBE << 4 | 0x02;
	
	//�̶�ͷ��----------------------ʣ�೤��ֵ-----------------------------------------------
	len = MQTT_DumpLength(remain_len, (uint8_t*)mqttPacket->_data + mqttPacket->_len);
	if(len < 0)
	{
		MQTT_DeleteBuffer(mqttPacket);
		return 4;
	}
	else
		mqttPacket->_len += len;
	
/*************************************payload***********************************************/
	
	//payload----------------------pkt_id---------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(pkt_id);
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(pkt_id);
	
	//payload----------------------topic_name-----------------------------------------------
	for(i = 0; i < topics_cnt; i++)
	{
		topic_len = strlen((char*)topics[i]);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(topic_len);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(topic_len);
		
		strncat((char*)mqttPacket->_data + mqttPacket->_len, (char*)topics[i], topic_len);
		mqttPacket->_len += topic_len;
		
		mqttPacket->_data[mqttPacket->_len++] = qos & 0xFF;
	}

	return 0;

}

//==========================================================
//	�������ƣ�	MQTT_UnPacketSubscrebe
//
//	�������ܣ�	Subscribe�Ļظ���Ϣ���
//
//	��ڲ�����	rev_data�����յ�����Ϣ
//
//	���ز�����	0-�ɹ�		����-ʧ��
//
//	˵����		
//==========================================================
uint8_t MQTT_UnPacketSubscribe(uint8_t *rev_data)
{
	
	uint8_t result = 255;

	if(rev_data[2] == MOSQ_MSB(MQTT_SUBSCRIBE_ID) && rev_data[3] == MOSQ_LSB(MQTT_SUBSCRIBE_ID))
	{
		switch(rev_data[4])
		{
			case 0x00:
			case 0x01:
			case 0x02:
				//MQTT Subscribe OK
				result = 0;
			break;
			
			case 0x80:
				//MQTT Subscribe Failed
				result = 1;
			break;
			
			default:
				//MQTT Subscribe UnKnown Err
				result = 2;
			break;
		}
	}
	
	return result;

}

//==========================================================
//	�������ƣ�	MQTT_PacketUnSubscribe
//
//	�������ܣ�	UnSubscribe��Ϣ���
//
//	��ڲ�����	pkt_id��pkt_id
//				qos����Ϣ�ط�����
//				topics�����ĵ���Ϣ
//				topics_cnt�����ĵ���Ϣ����
//				mqttPacket����ָ��
//
//	���ز�����	0-�ɹ�		����-ʧ��
//
//	˵����		
//==========================================================
uint8_t MQTT_PacketUnSubscribe(uint16_t pkt_id, const int8_t *topics[], uint8_t topics_cnt, MQTT_PACKET_STRUCTURE *mqttPacket)
{
	
	uint32_t topic_len = 0, remain_len = 0;
	int16_t len = 0;
	uint8_t i = 0;
	
	if(pkt_id == 0)
		return 1;
	
	//����topic����-------------------------------------------------------------------------
	for(; i < topics_cnt; i++)
	{
		if(topics[i] == NULL)
			return 2;
		
		topic_len += strlen((char*)topics[i]);
	}
	
	//2 bytes packet id, 2 bytes topic length + topic + 1 byte reserve---------------------
	remain_len = 2 + (topics_cnt << 1) + topic_len;
	
	//�����ڴ�------------------------------------------------------------------------------
	MQTT_NewBuffer(mqttPacket, remain_len + 5);
	if(mqttPacket->_data == NULL)
		return 3;
	
/*************************************�̶�ͷ��***********************************************/
	
	//�̶�ͷ��----------------------ͷ����Ϣ-------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_UNSUBSCRIBE << 4 | 0x02;
	
	//�̶�ͷ��----------------------ʣ�೤��ֵ-----------------------------------------------
	len = MQTT_DumpLength(remain_len, (uint8_t*)mqttPacket->_data + mqttPacket->_len);
	if(len < 0)
	{
		MQTT_DeleteBuffer(mqttPacket);
		return 4;
	}
	else
		mqttPacket->_len += len;
	
/*************************************payload***********************************************/
	
	//payload----------------------pkt_id---------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(pkt_id);
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(pkt_id);
	
	//payload----------------------topic_name-----------------------------------------------
	for(i = 0; i < topics_cnt; i++)
	{
		topic_len = strlen((char*)topics[i]);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(topic_len);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(topic_len);
		
		strncat((char*)mqttPacket->_data + mqttPacket->_len, (char*)topics[i], topic_len);
		mqttPacket->_len += topic_len;
	}

	return 0;

}

//==========================================================
//	�������ƣ�	MQTT_UnPacketUnSubscribe
//
//	�������ܣ�	UnSubscribe�Ļظ���Ϣ���
//
//	��ڲ�����	rev_data�����յ�����Ϣ
//
//	���ز�����	0-�ɹ�		����-ʧ��
//
//	˵����		
//==========================================================
uint8_t MQTT_UnPacketUnSubscribe(uint8_t *rev_data)
{
	
	uint8_t result = 1;

	if(rev_data[2] == MOSQ_MSB(MQTT_UNSUBSCRIBE_ID) && rev_data[3] == MOSQ_LSB(MQTT_UNSUBSCRIBE_ID))
	{
		result = 0;
	}
	
	return result;

}

//==========================================================
//	�������ƣ�	MQTT_PacketPublish
//
//	�������ܣ�	Pulish��Ϣ���
//
//	��ڲ�����	pkt_id��pkt_id
//				topic��������topic
//				payload����Ϣ��
//				payload_len����Ϣ�峤��
//				qos���ط�����
//				retain��������Ϣ����
//				own��
//				mqttPacket����ָ��
//
//	���ز�����	0-�ɹ�		����-ʧ��
//
//	˵����		
//==========================================================
uint8_t MQTT_PacketPublish(uint16_t pkt_id, const char *topic,
						const char *payload, uint32_t payload_len,
						enum MqttQosLevel qos, int32_t retain, int32_t own,
						MQTT_PACKET_STRUCTURE *mqttPacket)
{

	uint32_t total_len = 0, topic_len = 0;
	uint32_t data_len = 0;
	int32_t len = 0;
	uint8_t flags = 0;
	
	//pkt_id���----------------------------------------------------------------------------
	if(pkt_id == 0)
		return 1;
	
	//$dpΪϵͳ�ϴ����ݵ��ָ��--------------------------------------------------------------
	for(topic_len = 0; topic[topic_len] != '\0'; ++topic_len)
	{
		if((topic[topic_len] == '#') || (topic[topic_len] == '+'))
			return 2;
	}
	
	//Publish��Ϣ---------------------------------------------------------------------------
	flags |= MQTT_PKT_PUBLISH << 4;
	
	//retain��־----------------------------------------------------------------------------
	if(retain)
		flags |= 0x01;
	
	//�ܳ���--------------------------------------------------------------------------------
	total_len = topic_len + payload_len + 2;
	
	//qos����--��Ҫ����PUBLISH������̬����Ϣ�ģ���֤��Ϣ���ݵĴ���-----------------------------
	switch(qos)
	{
		case MQTT_QOS_LEVEL0:
			flags |= MQTT_CONNECT_WILL_QOS0;	//���һ��
		break;
		
		case MQTT_QOS_LEVEL1:
			flags |= 0x02;						//����һ��
			total_len += 2;
		break;
		
		case MQTT_QOS_LEVEL2:
			flags |= 0x04;						//ֻ��һ��
			total_len += 2;
		break;
		
		default:
		return 3;
	}
	
	//�����ڴ�------------------------------------------------------------------------------
	if(payload != NULL)
	{
		if(payload[0] == 2)
		{
			uint32_t data_len_t = 0;
			
			while(payload[data_len_t++] != '}');
			data_len_t -= 3;
			data_len = data_len_t + 7;
			data_len_t = payload_len - data_len;
			
			MQTT_NewBuffer(mqttPacket, total_len + 3 - data_len_t);
			
			if(mqttPacket->_data == NULL)
				return 4;
			
			memset(mqttPacket->_data, 0, total_len + 3 - data_len_t);
		}
		else
		{
			MQTT_NewBuffer(mqttPacket, total_len + 5);
			
			if(mqttPacket->_data == NULL)
				return 4;
			
			memset(mqttPacket->_data, 0, total_len + 5);
		}
	}
	else
	{
		MQTT_NewBuffer(mqttPacket, total_len + 5);
		
		if(mqttPacket->_data == NULL)
			return 4;
		
		memset(mqttPacket->_data, 0, total_len + 5);
	}
	
/*************************************�̶�ͷ��***********************************************/
	
	//�̶�ͷ��----------------------ͷ����Ϣ-------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = flags;
	
	//�̶�ͷ��----------------------ʣ�೤��ֵ-----------------------------------------------
	len = MQTT_DumpLength(total_len, (uint8_t*)mqttPacket->_data + mqttPacket->_len);
	if(len < 0)
	{
		MQTT_DeleteBuffer(mqttPacket);
		return 5;
	}
	else
		mqttPacket->_len += len;
	
/*************************************�ɱ�ͷ��***********************************************/
	
	//�ɱ�ͷ��----------------------д��topic���ȡ�topic-------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(topic_len);
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(topic_len);
	
	strncat((char*)mqttPacket->_data + mqttPacket->_len, (char*)topic, topic_len);
	mqttPacket->_len += topic_len;
	if(qos != MQTT_QOS_LEVEL0)
	{
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(pkt_id);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(pkt_id);
	}
	
	//�ɱ�ͷ��----------------------д��payload----------------------------------------------
	if(payload != NULL)
	{
		if(payload[0] == 2)
		{
			memcpy((int8_t *)mqttPacket->_data + mqttPacket->_len, payload, data_len);
			mqttPacket->_len += data_len;
		}
		else
		{
			memcpy((int8_t *)mqttPacket->_data + mqttPacket->_len, payload, payload_len);
			mqttPacket->_len += payload_len;
		}
	}
	
	return 0;

}

//==========================================================
//	�������ƣ�	MQTT_UnPacketPublish
//
//	�������ܣ�	Publish��Ϣ���
//
//	��ڲ�����	flags��MQTT��ر�־��Ϣ
//				pkt��ָ��ɱ�ͷ��
//				size���̶�ͷ���е�ʣ�೤����Ϣ
//
//	���ز�����	0-�ɹ�		����-ʧ��ԭ��
//
//	˵����		
//==========================================================

uint8_t MQTT_UnPacketPublish(uint8_t *rev_data, char **topic, uint16_t *topic_len, char **payload, uint16_t *payload_len, uint8_t *qos, uint16_t *pkt_id)
{
	
	const int8_t flags = rev_data[0] & 0x0F;
	uint8_t *msgPtr;
	uint32_t remain_len = 0;

	const int8_t dup = flags & 0x08;

	*qos = (flags & 0x06) >> 1;
	
	msgPtr = rev_data + MQTT_ReadLength(rev_data + 1, 4, &remain_len) + 1;
	
	if(remain_len < 2 || flags & 0x01)							//retain
		return 255;
	
	*topic_len = (uint16_t)msgPtr[0] << 8 | msgPtr[1];
	if(remain_len < *topic_len + 2)
		return 255;
	
	if(strstr((char*)msgPtr + 2, CMD_TOPIC_PREFIX) != NULL)	//����������·�
		return MQTT_PKT_CMD;
	
	switch(*qos)
	{
		case MQTT_QOS_LEVEL0:									// qos0 have no packet identifier
			
			if(0 != dup)
				return 255;

			*topic = MQTT_MallocBuffer(*topic_len + 1);			//Ϊtopic�����ڴ�
			if(*topic == NULL)
				return 255;
			
			memset(*topic, 0, *topic_len + 1);
			memcpy(*topic, (int8_t *)msgPtr + 2, *topic_len);		//��������
			
			*payload_len = remain_len - 2 - *topic_len;			//Ϊpayload�����ڴ�
			*payload = MQTT_MallocBuffer(*payload_len + 1);
			if(*payload == NULL)								//���ʧ��
			{
				MQTT_FreeBuffer(*topic);						//����Ҫ��topic���ڴ��ͷŵ�
				return 255;
			}
			
			memset(*payload, 0, *payload_len + 1);
			memcpy(*payload, (int8_t *)msgPtr + 2 + *topic_len, *payload_len);
			
		break;

		case MQTT_QOS_LEVEL1:
		case MQTT_QOS_LEVEL2:
			
			if(*topic_len + 2 > remain_len)
				return 255;
			
			*pkt_id = (uint16_t)msgPtr[*topic_len + 2] << 8 | msgPtr[*topic_len + 3];
			if(pkt_id == 0)
				return 255;
			
			*topic = MQTT_MallocBuffer(*topic_len + 1);			//Ϊtopic�����ڴ�
			if(*topic == NULL)
				return 255;
			
			memset(*topic, 0, *topic_len + 1);
			memcpy(*topic, (int8_t *)msgPtr + 2, *topic_len);		//��������
			
			*payload_len = remain_len - 4 - *topic_len;
			*payload = MQTT_MallocBuffer(*payload_len + 1);		//Ϊpayload�����ڴ�
			if(*payload == NULL)								//���ʧ��
			{
				MQTT_FreeBuffer(*topic);						//����Ҫ��topic���ڴ��ͷŵ�
				return 255;
			}
			
			memset(*payload, 0, *payload_len + 1);
			memcpy(*payload, (int8_t *)msgPtr + 4 + *topic_len, *payload_len);
			
		break;

		default:
			return 255;
	}
	
	if(strchr((char *)topic, '+') || strchr((char *)topic, '#'))
		return 255;

	return 0;

}

//==========================================================
//	�������ƣ�	MQTT_PacketPublishAck
//
//	�������ܣ�	Publish Ack��Ϣ���
//
//	��ڲ�����	pkt_id��packet id
//				mqttPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��ԭ��
//
//	˵����		���յ���Publish��Ϣ��QoS�ȼ�Ϊ1ʱ����ҪAck�ظ�
//==========================================================
uint8_t MQTT_PacketPublishAck(uint16_t pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 4);
	if(mqttPacket->_data == NULL)
		return 1;
	
/*************************************�̶�ͷ��***********************************************/
	
	//�̶�ͷ��----------------------ͷ����Ϣ-------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_PUBACK << 4;
	
	//�̶�ͷ��----------------------ʣ�೤��-------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = 2;
	
/*************************************�ɱ�ͷ��***********************************************/
	
	//�ɱ�ͷ��----------------------pkt_id����-----------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = pkt_id >> 8;
	mqttPacket->_data[mqttPacket->_len++] = pkt_id & 0xff;
	
	return 0;

}

//==========================================================
//	�������ƣ�	MQTT_UnPacketPublishAck
//
//	�������ܣ�	Publish Ack��Ϣ���
//
//	��ڲ�����	rev_data���յ�������
//
//	���ز�����	0-�ɹ�		1-ʧ��ԭ��
//
//	˵����		
//==========================================================
uint8_t MQTT_UnPacketPublishAck(uint8_t *rev_data)
{

	if(rev_data[1] != 2)
		return 1;

	if(rev_data[2] == MOSQ_MSB(MQTT_PUBLISH_ID) && rev_data[3] == MOSQ_LSB(MQTT_PUBLISH_ID))
		return 0;
	else
		return 1;

}

//==========================================================
//	�������ƣ�	MQTT_PacketPublishRec
//
//	�������ܣ�	Publish Rec��Ϣ���
//
//	��ڲ�����	pkt_id��packet id
//				mqttPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��ԭ��
//
//	˵����		���յ���Publish��Ϣ��QoS�ȼ�Ϊ2ʱ�����յ�rec
//==========================================================
uint8_t MQTT_PacketPublishRec(uint16_t pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 4);
	if(mqttPacket->_data == NULL)
		return 1;
	
/*************************************�̶�ͷ��***********************************************/
	
	//�̶�ͷ��----------------------ͷ����Ϣ-------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_PUBREC << 4;
	
	//�̶�ͷ��----------------------ʣ�೤��-------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = 2;
	
/*************************************�ɱ�ͷ��***********************************************/
	
	//�ɱ�ͷ��----------------------pkt_id����-----------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = pkt_id >> 8;
	mqttPacket->_data[mqttPacket->_len++] = pkt_id & 0xff;
	
	return 0;

}

//==========================================================
//	�������ƣ�	MQTT_UnPacketPublishRec
//
//	�������ܣ�	Publish Rec��Ϣ���
//
//	��ڲ�����	rev_data�����յ�������
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint8_t MQTT_UnPacketPublishRec(uint8_t *rev_data)
{

	if(rev_data[1] != 2)
		return 1;

	if(rev_data[2] == MOSQ_MSB(MQTT_PUBLISH_ID) && rev_data[3] == MOSQ_LSB(MQTT_PUBLISH_ID))
		return 0;
	else
		return 1;

}

//==========================================================
//	�������ƣ�	MQTT_PacketPublishRel
//
//	�������ܣ�	Publish Rel��Ϣ���
//
//	��ڲ�����	pkt_id��packet id
//				mqttPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��ԭ��
//
//	˵����		���յ���Publish��Ϣ��QoS�ȼ�Ϊ2ʱ�����յ�rec���ٻظ�rel
//==========================================================
uint8_t MQTT_PacketPublishRel(uint16_t pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 4);
	if(mqttPacket->_data == NULL)
		return 1;
	
/*************************************�̶�ͷ��***********************************************/
	
	//�̶�ͷ��----------------------ͷ����Ϣ-------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_PUBREL << 4 | 0x02;
	
	//�̶�ͷ��----------------------ʣ�೤��-------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = 2;
	
/*************************************�ɱ�ͷ��***********************************************/
	
	//�ɱ�ͷ��----------------------pkt_id����-----------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = pkt_id >> 8;
	mqttPacket->_data[mqttPacket->_len++] = pkt_id & 0xff;
	
	return 0;

}

//==========================================================
//	�������ƣ�	MQTT_UnPacketPublishRel
//
//	�������ܣ�	Publish Rel��Ϣ���
//
//	��ڲ�����	rev_data�����յ�������
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint8_t MQTT_UnPacketPublishRel(uint8_t *rev_data, uint16_t pkt_id)
{

	if(rev_data[1] != 2)
		return 1;

	if(rev_data[2] == MOSQ_MSB(pkt_id) && rev_data[3] == MOSQ_LSB(pkt_id))
		return 0;
	else
		return 1;

}

//==========================================================
//	�������ƣ�	MQTT_PacketPublishComp
//
//	�������ܣ�	Publish Comp��Ϣ���
//
//	��ڲ�����	pkt_id��packet id
//				mqttPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��ԭ��
//
//	˵����		���յ���Publish��Ϣ��QoS�ȼ�Ϊ2ʱ�����յ�rec���ٻظ�rel
//==========================================================
uint8_t MQTT_PacketPublishComp(uint16_t pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 4);
	if(mqttPacket->_data == NULL)
		return 1;
	
/*************************************�̶�ͷ��***********************************************/
	
	//�̶�ͷ��----------------------ͷ����Ϣ-------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_PUBCOMP << 4;
	
	//�̶�ͷ��----------------------ʣ�೤��-------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = 2;
	
/*************************************�ɱ�ͷ��***********************************************/
	
	//�ɱ�ͷ��----------------------pkt_id����-----------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = pkt_id >> 8;
	mqttPacket->_data[mqttPacket->_len++] = pkt_id & 0xff;
	
	return 0;

}

//==========================================================
//	�������ƣ�	MQTT_UnPacketPublishComp
//
//	�������ܣ�	Publish Comp��Ϣ���
//
//	��ڲ�����	rev_data�����յ�������
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint8_t MQTT_UnPacketPublishComp(uint8_t *rev_data)
{

	if(rev_data[1] != 2)
		return 1;

	if(rev_data[2] == MOSQ_MSB(MQTT_PUBLISH_ID) && rev_data[3] == MOSQ_LSB(MQTT_PUBLISH_ID))
		return 0;
	else
		return 1;

}

//==========================================================
//	�������ƣ�	MQTT_PacketPing
//
//	�������ܣ�	�����������
//
//	��ڲ�����	mqttPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint8_t MQTT_PacketPing(MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 2);
	if(mqttPacket->_data == NULL)
		return 1;
	
/*************************************�̶�ͷ��***********************************************/
	
	//�̶�ͷ��----------------------ͷ����Ϣ-------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_PINGREQ << 4;
	
	//�̶�ͷ��----------------------ʣ�೤��-------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = 0;
	
	return 0;

}