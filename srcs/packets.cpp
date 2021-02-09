/*
 * @Author: sinpo828
 * @Date: 2021-02-04 14:04:11
 * @LastEditors: sinpo828
 * @LastEditTime: 2021-02-09 15:12:44
 * @Description: file content
 */
#include <iostream>
#include <string>

#include <cstring>

#include "packets.hpp"

const static uint8_t MAGIC_7e = 0x7e;
const static uint8_t MAGIC_7d = 0x7d;
const static uint8_t MAGIC_5e = 0x5e;
const static uint8_t MAGIC_5d = 0x5d;
#define FRAMEHDR(p) (reinterpret_cast<cmd_header *>(p))
#define FRAMEDATA(p, n, t) (reinterpret_cast<t *>(p + n))
#define FRAMETAIL(p, n) (reinterpret_cast<cmd_tail *>(p + n))

Command::Command(const std::string &v) : modem_name(v)
{
    _data = new uint8_t[max_data_len];
}

Command::~Command()
{
    if (_data)
        delete[] _data;

    _data = nullptr;
}

std::string Command::cmdstr()
{
    cmd_header *hdr = FRAMEHDR(_data);

    if (_reallen == 1)
        return "BSL_CMD_CHECK_BAUD";

    switch (static_cast<REQTYPE>(be16toh(hdr->cmd_type)))
    {
    case REQTYPE::BSL_CMD_CONNECT:
        return "BSL_CMD_CONNECT";
    case REQTYPE::BSL_CMD_START_DATA:
        return "BSL_CMD_START_DATA";
    case REQTYPE::BSL_CMD_MIDST_DATA:
        return "BSL_CMD_MIDST_DATA";
    case REQTYPE::BSL_CMD_END_DATA:
        return "BSL_CMD_END_DATA";
    case REQTYPE::BSL_CMD_EXEC_DATA:
        return "BSL_CMD_EXEC_DATA";
    case REQTYPE::BSL_CMD_NORMAL_RESET:
        return "BSL_CMD_NORMAL_RESET";
    case REQTYPE::BSL_CMD_READ_FLASH:
        return "BSL_CMD_READ_FLASH";
    case REQTYPE::BSL_CMD_CHANGE_BAUD:
        return "BSL_CMD_CHANGE_BAUD";
    case REQTYPE::BSL_CMD_ERASE_FLASH:
        return "BSL_CMD_ERASE_FLASH";
    default:
        return "UNKNOW_COMMAND";
    }
}

uint8_t *Command::data()
{
    return _data;
}

uint32_t Command::datalen()
{
    return _reallen;
}

uint8_t *Command::rawdata()
{
    return _data + sizeof(cmd_header);
}

uint32_t Command::rawdatalen()
{
    auto hdr = FRAMEHDR(_data);
    return be16toh(hdr->data_length);
}

uint16_t Command::crc16(char *src, uint32_t len)
{
#define CRC_16_POLYNOMIAL 0x1021
#define CRC_16_L_POLYNOMIAL 0x8000
#define CRC_16_L_SEED 0x80
#define CRC_16_L_OK 0x00
#define CRC_CHECK_SIZE 0x02

    unsigned int i;
    uint16_t crc = 0;
    while (len-- != 0)
    {
        for (i = CRC_16_L_SEED; i != 0; i = i >> 1)
        {
            if ((crc & CRC_16_L_POLYNOMIAL) != 0)
            {
                crc = crc << 1;
                crc = crc ^ CRC_16_POLYNOMIAL;
            }
            else
                crc = crc << 1;
            if ((*src & i) != 0)
                crc = crc ^ CRC_16_POLYNOMIAL;
        }
        src++;
    }
    return (crc);
}

uint16_t Command::frm_chk(uint16_t *src, uint32_t len)
{
    unsigned int sum = 0;
    uint16_t SourceValue, DestValue;
    uint16_t lowSourceValue, hiSourceValue;
    /* Get sum value of the source.*/
    while (len > 1)
    {

        SourceValue = *src++;
        DestValue = 0;
        lowSourceValue = (SourceValue & 0xFF00) >> 8;
        hiSourceValue = (SourceValue & 0x00FF) << 8;
        DestValue = lowSourceValue | hiSourceValue;
        sum += DestValue;
        len -= 2;
    }
    if (len == 1)
    {
        sum += *((unsigned char *)src);
    }

    sum = (sum >> 16) + (sum & 0x0FFFF);
    sum += (sum >> 16);

    return (~sum);
}

uint16_t Command::crc16(uint16_t crc, uint8_t *buffer, uint32_t len)
{
    /** CRC table for the CRC-16. The poly is 0x8005 (x^16 + x^15 + x^2 + 1) */
    uint16_t const crc16_table[256] = {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040};

    while (len--)
        crc = (uint16_t)((crc >> 8) ^ (crc16_table[(crc ^ (*buffer++)) & 0xff]));
    return crc;
}

void Command::reinit(REQTYPE req)
{
    cmd_header *hdr = FRAMEHDR(_data);

    hdr->magic = MAGIC_7e;
    hdr->cmd_type = htobe16(static_cast<uint16_t>(req));
    hdr->data_length = 0;

    _reallen = sizeof(cmd_header);
}

void Command::finishup()
{
    cmd_tail *tail = FRAMETAIL(_data, _reallen);

    tail->crc16 = htobe16(frm_chk((uint16_t *)(_data + 1), _reallen - 1 - sizeof(cmd_tail)));
    tail->magic = MAGIC_7e;

    _reallen += sizeof(cmd_tail);
}

template <typename T>
void Command::push_back(T val)
{
    cmd_header *hdr = FRAMEHDR(_data);

    switch (sizeof(T))
    {
    case 1:
        *FRAMEDATA(_data, _reallen, T) = val;
        break;

    case 2:
        *FRAMEDATA(_data, _reallen, T) = htobe16(val);
        break;

    case 4:
        *FRAMEDATA(_data, _reallen, T) = htobe32(val);
        break;

    case 8:
        *FRAMEDATA(_data, _reallen, T) = htobe64(val);
        break;

    default:
        std::cerr << "unknow type with sizeof " << sizeof(T) << std::endl;
        break;
    }

    _reallen += sizeof(T);
    hdr->data_length = htobe16(_reallen - sizeof(cmd_header));
}

void Command::newCheckBaud()
{
    _data[0] = 0x7e;
    _reallen = 1;
}

void Command::newConnect()
{
    reinit(REQTYPE::BSL_CMD_CONNECT);
    finishup();
}

void Command::newStartData(uint32_t addr, uint32_t len)
{
    reinit(REQTYPE::BSL_CMD_START_DATA);
    push_back(addr);
    push_back(len);
    finishup();
}

/**
 * 7e -> 7d 5e
 * 7d -> 7d 5d
 */
void Command::newMidstData(uint8_t *buf, uint32_t len)
{
    cmd_header *hdr = FRAMEHDR(_data);

    reinit(REQTYPE::BSL_CMD_MIDST_DATA);
    for (int pos = 0; pos < len; pos++)
    {
        if (buf[pos] == MAGIC_7e)
        {
            _data[_reallen++] = MAGIC_7d;
            _data[_reallen++] = MAGIC_5e;
        }
        else if (buf[pos] == MAGIC_7d)
        {
            _data[_reallen++] = MAGIC_7d;
            _data[_reallen++] = MAGIC_5d;
        }
        else
        {
            _data[_reallen++] = buf[pos];
        }
    }

    hdr->data_length = htobe16(_reallen - sizeof(cmd_header));
    finishup();
}

void Command::newEndData()
{
    reinit(REQTYPE::BSL_CMD_END_DATA);
    finishup();
}

void Command::newChangeBaud(BAUD baud)
{
    reinit(REQTYPE::BSL_CMD_CHANGE_BAUD);
    push_back(static_cast<uint32_t>(baud));
    finishup();
}

Response::Response()
{
    _data = new uint8_t[max_data_len];
}

Response ::~Response()
{
    if (_data)
        delete[] _data;
}

std::string Response::respstr()
{
    auto hdr = FRAMEHDR(_data);
    switch (static_cast<REPTYPE>(be16toh(hdr->cmd_type)))
    {
    case REPTYPE::BSL_REP_ACK:
        return "BSL_REP_ACK";
    case REPTYPE::BSL_REP_VER:
        return "BSL_REP_VER";
    case REPTYPE::BSL_REP_INVALID_CMD:
        return "BSL_REP_INVALID_CMD";
    case REPTYPE::BSL_REP_UNKNOW_CMD:
        return "BSL_REP_UNKNOW_CMD";
    case REPTYPE::BSL_REP_OPERATION_FAILED:
        return "BSL_REP_OPERATION_FAILED";
    case REPTYPE::BSL_REP_NOT_SUPPORT_BAUDRATE:
        return "BSL_REP_NOT_SUPPORT_BAUDRATE";
    case REPTYPE::BSL_REP_DOWN_NOT_START:
        return "BSL_REP_DOWN_NOT_START";
    case REPTYPE::BSL_REP_DOWN_MUTI_START:
        return "BSL_REP_DOWN_MUTI_START";
    case REPTYPE::BSL_REP_DOWN_EARLY_END:
        return "BSL_REP_DOWN_EARLY_END";
    case REPTYPE::BSL_REP_DOWN_DEST_ERROR:
        return "BSL_REP_DOWN_DEST_ERROR";
    case REPTYPE::BSL_REP_DOWN_SIZE_ERROR:
        return "BSL_REP_DOWN_SIZE_ERROR";
    case REPTYPE::BSL_REP_VERIFY_ERROR:
        return "BSL_REP_VERIFY_ERROR";
    case REPTYPE::BSL_REP_NOT_VERIFY:
        return "BSL_REP_NOT_VERIFY";
    case REPTYPE::BSL_REP_READ_FLASH:
        return "BSL_REP_READ_FLASH";
    default:
        return "UNKNOW_RESPONSE";
    }
}

/**
 * 7d 5e -> 7e
 * 7d 5d -> 7d
 */
void Response::parser(uint8_t *d, uint32_t len)
{
    _reallen = sizeof(cmd_header);
    memcpy(_data, d, _reallen);

    for (int pos = _reallen; pos < len - sizeof(cmd_tail); pos++)
    {
        if (d[pos] == MAGIC_7d && d[pos + 1] == MAGIC_5e)
        {
            _data[_reallen++] = MAGIC_7e;
            pos++;
        }
        else if (d[pos] == MAGIC_7d && d[pos] == MAGIC_5d)
        {
            _data[_reallen++] = MAGIC_7d;
            pos++;
        }
        else
        {
            _data[_reallen++] = d[pos];
        }
    }

    memcpy(_data + _reallen, d + len - sizeof(cmd_tail), sizeof(cmd_tail));
    _reallen += sizeof(cmd_tail);
}

void Response::reset()
{
    memset(_data, 0, max_data_len);
    _reallen = 0;
}

REPTYPE Response::cmdtype()
{
    auto hdr = FRAMEHDR(_data);

    return static_cast<REPTYPE>(be16toh(hdr->cmd_type));
}

uint8_t *Response::data()
{
    return _data;
}

uint32_t Response::datalen()
{
    return _reallen;
}

uint8_t *Response::rawdata()
{
    return _data + sizeof(cmd_header);
}

uint32_t Response::rawdatalen()
{
    auto hdr = FRAMEHDR(_data);
    return be16toh(hdr->data_length);
}
