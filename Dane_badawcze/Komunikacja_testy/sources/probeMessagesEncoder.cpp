#include "probeMessagesEncoder.h"
#include <time.h>
#include <chrono>
#include <cstring>
using namespace std;


ProbeMessagesEncoder::message::message(ProbeMessagesEncoder::message::frame_type_t type)
{
    head.frame_id = type;
}

bool ProbeMessagesEncoder::message::operator==(const ProbeMessagesEncoder::message &rhs)
{
    if(head.frame_id != rhs.head.frame_id) return false;
    if(head.dev_unique_id != rhs.head.dev_unique_id) return false;
    if(head.dev_version != rhs.head.dev_version) return false;
    return true;
}

ProbeMessagesEncoder::message::~message()
{

}

bool ProbeMessagesEncoder::message::datetime::operator ==(const ProbeMessagesEncoder::message::datetime &other)
{
    if(this->sygn_time_TOW_var != other.sygn_time_TOW_var) return false;
    if(this->TOW != other.TOW) return false;
    if(this->TOW_F != other.TOW_F) return false;
    if(this->WN != other.WN) return false;
    if(this->sync_status != other.sync_status) return false;
    return true;
}

ProbeMessagesEncoder::messageThreat::messageThreat() :
    message(message::FRAME_THREAT),
    threats_number(0)
{
    threats_data = new threat_descriptor_t[MAX_THREATS_IN_SINGLE_MESSAGE];
}

ProbeMessagesEncoder::messageThreat::~messageThreat()
{
    delete [] threats_data;
}

bool ProbeMessagesEncoder::messageThreat::operator==(const messageThreat &rhs)
{
    if(this->isEmpty()) return false;
    if(rhs.isEmpty()) return false;
    if(this->isEmpty() || rhs.isEmpty()) return false;
    if(this->countThreats() != rhs.countThreats()) return false;
    uint8_t n = this->countThreats();
    for(uint8_t i=0;i<n;i++)
    {
        if(this->getThreats()[i] != rhs.getThreats()[i]) return false;
    }
    return true;
}

bool ProbeMessagesEncoder::messageThreat::appendThreat(const ProbeMessagesEncoder::messageThreat::threat_descriptor_t &threat)
{
    if(this->isFull()) return false;
    this->threats_data[this->threats_number] = threat;
    this->threats_number++;
    return true;
}

bool ProbeMessagesEncoder::messageThreat::threat_descriptor::operator ==(const ProbeMessagesEncoder::messageThreat::threat_descriptor &other)
{
    for(uint8_t i=0;i< sizeof(this->head)/sizeof(this->head[0]);++i) {
        if(this->head[i] != other.head[i]) return false;
    }
    if(sygn_BW != other.sygn_BW) return false;
    if(sygn_BW_var != other.sygn_BW_var) return false;
    if(sygn_f0 != other.sygn_f0) return false;
    if(sygn_f0_var != other.sygn_f0_var) return false;
    if(sygn_J != other.sygn_J) return false;
    if(sygn_No != other.sygn_No) return false;
    if(!(sygn_time == other.sygn_time)) return false;
    if(tail != other.tail) return false;
    return true;
}

bool ProbeMessagesEncoder::messageThreat::chirp_threat_descriptor::operator ==(const ProbeMessagesEncoder::messageThreat::chirp_threat_descriptor &other)
{
    if(sygn_typ != other.sygn_typ) return false;
    if(sygn_chirp_period != other.sygn_chirp_period) return false;
    if(sygn_chirp_return != other.sygn_chirp_return) return false;
    if(sygn_chirp_sweep != other.sygn_chirp_sweep) return false;
    if(reserved != other.reserved) return false;

    return true;
}


bool ProbeMessagesEncoder::messageThreat::pulse_threat_descriptor::operator ==(const ProbeMessagesEncoder::messageThreat::pulse_threat_descriptor &other)
{
    if(sygn_typ != other.sygn_typ) return false;
    if(sygn_pulse_p1 != other.sygn_pulse_p1) return false;
    if(sygn_pulse_p2 != other.sygn_pulse_p2) return false;
    if(sygn_pulse_p3 != other.sygn_pulse_p3) return false;
    if(sygn_pulse_p4 != other.sygn_pulse_p4) return false;
    if(sygn_pulse_s1 != other.sygn_pulse_s1) return false;
    if(sygn_pulse_s2 != other.sygn_pulse_s2) return false;
    if(sygn_pulse_s3 != other.sygn_pulse_s3) return false;
    if(reserved != other.reserved) return false;

    return true;
}

void ProbeMessagesEncoder::messageFFT::fft_descriptor::allocate(uint8_t exponent)
{
    clearBuffer();
    fft_N_exponent = exponent;
    uint32_t N = ProbeMessagesEncoder::messageFFT::exponentToFFTlength(fft_N_exponent);
    fft_power_density = new int8_t[N];
}

void ProbeMessagesEncoder::messageFFT::fft_descriptor::clearBuffer()
{
    if(fft_power_density != nullptr)
    {
        delete [] fft_power_density;
    }
    fft_power_density = nullptr;
}

ProbeMessagesEncoder::messageFFT::fft_descriptor::fft_descriptor()
{

}

ProbeMessagesEncoder::messageFFT::fft_descriptor::~fft_descriptor()
{
    clearBuffer();
}

bool ProbeMessagesEncoder::messageFFT::fft_descriptor::operator ==(const ProbeMessagesEncoder::messageFFT::fft_descriptor &other)
{
    if(fft_power_density == nullptr) return false;
    if(other.fft_power_density == nullptr) return false;
    if(fft_f_begin != other.fft_f_begin) return false;
    if(fft_sampling_rate != other.fft_sampling_rate) return false;
    if(fft_N_exponent != other.fft_N_exponent) return false;
    uint32_t N = ProbeMessagesEncoder::messageFFT::exponentToFFTlength(fft_N_exponent);
    for(uint32_t i=0;i<N;i++)
    {
        if(fft_power_density[i] != other.fft_power_density[i]) return false;
    }
    return true;
}

ProbeMessagesEncoder::messageFFT::fft_descriptor::fft_descriptor(const ProbeMessagesEncoder::messageFFT::fft_descriptor &other)
{
    this->clearBuffer();
    this->fft_f_begin = other.fft_f_begin;
    this->fft_sampling_rate = other.fft_sampling_rate;
    if(other.fft_power_density == nullptr) return;

    this->allocate(other.fft_N_exponent);
    uint32_t N = other.count();
    for(uint32_t i=0;i<N;i++)
    {
        this->fft_power_density[i] = other.fft_power_density[i];
    }
}

void ProbeMessagesEncoder::messageFFT::fft_descriptor::operator = (const fft_descriptor &other)
{
    this->clearBuffer();
    this->fft_f_begin = other.fft_f_begin;
    this->fft_sampling_rate = other.fft_sampling_rate;
    if(other.fft_power_density == nullptr) return;

    this->allocate(other.fft_N_exponent);
    uint32_t N = other.count();
    for(uint32_t i=0;i<N;i++)
    {
        this->fft_power_density[i] = other.fft_power_density[i];
    }
}


void ProbeMessagesEncoder::messageFFT::setFFT(const ProbeMessagesEncoder::messageFFT::fft_descriptor_t &data)
{
    this->clear();
    this->fft_data = new fft_descriptor_t(data);
}

void ProbeMessagesEncoder::messageFFT::clear()
{
    if(fft_data != nullptr) delete fft_data;
    fft_data = nullptr;
}

ProbeMessagesEncoder::messageFFT::messageFFT() : message(FRAME_FFT)
{

}

ProbeMessagesEncoder::messageFFT::~messageFFT()
{
    clear();
}


uint32_t ProbeMessagesEncoder::pack_int16(ProbeMessagesEncoder::buffer_t *destination_buffer, uint32_t index, int16_t value)
{
    destination_buffer[index] = static_cast<uint8_t>(value >> 8);
    ++index;
    destination_buffer[index] = static_cast<uint8_t>(value & 0x00FF);
    ++index;
    return index;
}

uint32_t ProbeMessagesEncoder::pack_int16(ProbeMessagesEncoder::buffer_t *destination_buffer, uint32_t index, int16_t *values, uint32_t n)
{
    uint32_t i,j;
    for(i=0,j=index;i<n;++i,j+=2) this->pack_int16(destination_buffer,j,values[i]);
    return j;
}

int16_t ProbeMessagesEncoder::unpack_int16(ProbeMessagesEncoder::buffer_t **source_buffer)
{
    int16_t value = static_cast<int16_t>(**source_buffer) << 8; (*source_buffer)++;
    value |= static_cast<int16_t>(**source_buffer) & 0x00FF; (*source_buffer)++;
    return value;
}

void ProbeMessagesEncoder::unpack_int16(ProbeMessagesEncoder::buffer_t **source_buffer, int16_t *destination_buffer, uint32_t n)
{
    for(uint32_t i=0;i<n;i++,destination_buffer++) *destination_buffer = this->unpack_int16(source_buffer);
}

uint32_t ProbeMessagesEncoder::pack_uint24(ProbeMessagesEncoder::buffer_t *destination_buffer, uint32_t index, uint32_t value)
{
    for(uint8_t i=0;i<3;i++)
    {
        uint8_t offset = (2-i)*8;
        uint32_t mask = (static_cast<uint32_t>(0xFF) << offset);
        destination_buffer[index++] = static_cast<uint8_t>((value & mask) >> offset);
    }
    return index;
}

uint32_t ProbeMessagesEncoder::pack_uint24(ProbeMessagesEncoder::buffer_t *destination_buffer, uint32_t index, uint32_t *values, uint32_t n)
{
    uint32_t i,j;
    for(i=0,j=index;i<n;++i,j+=3) this->pack_uint24(destination_buffer,j,values[i]);
    return j;
}

uint32_t ProbeMessagesEncoder::unpack_uint24(ProbeMessagesEncoder::buffer_t **source_buffer)
{
    uint32_t value=0;
    for(uint8_t i=0;i<3;i++)
    {
        uint8_t offset = (2-i)*8;
        uint32_t mask = (static_cast<uint32_t>(0xFF) << offset);
        value |= ((static_cast<uint32_t>(**source_buffer) << offset) & mask);
        (*source_buffer)++;
    }
    return value;
}

void ProbeMessagesEncoder::unpack_uint24(ProbeMessagesEncoder::buffer_t **source_buffer, uint32_t *destination_buffer, uint32_t n)
{
    for(uint32_t i=0;i<n;i++,destination_buffer++) *destination_buffer = this->unpack_uint24(source_buffer);
}

uint32_t ProbeMessagesEncoder::pack_uint32(ProbeMessagesEncoder::buffer_t *destination_buffer, uint32_t index, uint32_t value)
{
    for(uint8_t i=0;i<4;i++)
    {
        uint8_t offset = (3-i)*8;
        uint32_t mask = (static_cast<uint32_t>(0xFF) << offset);
        destination_buffer[index++] = static_cast<uint8_t>((value & mask) >> offset);
    }
    return index;
}

uint32_t ProbeMessagesEncoder::pack_uint32(ProbeMessagesEncoder::buffer_t *destination_buffer, uint32_t index, uint32_t *values, uint32_t n)
{
    uint32_t i,j;
    for(i=0,j=index;i<n;++i,j+=4) this->pack_uint32(destination_buffer,j,values[i]);
    return j;
}

uint32_t ProbeMessagesEncoder::unpack_uint32(ProbeMessagesEncoder::buffer_t **source_buffer)
{
    uint32_t value=0;
    for(uint8_t i=0;i<4;i++)
    {
        uint8_t offset = (7-i)*8;
        uint32_t mask = (static_cast<uint32_t>(0xFF) << offset);
        value |= ((static_cast<uint32_t>(**source_buffer) << offset) & mask);
        (*source_buffer)++;
    }
    return value;
}

void ProbeMessagesEncoder::unpack_uint32(ProbeMessagesEncoder::buffer_t **source_buffer, uint32_t *destination_buffer, uint32_t n)
{
    for(uint32_t i=0;i<n;i++,destination_buffer++) *destination_buffer = this->unpack_uint32(source_buffer);
}

uint32_t ProbeMessagesEncoder::pack_uint64(ProbeMessagesEncoder::buffer_t *destination_buffer, uint32_t index, uint64_t value)
{
    for(uint8_t i=0;i<8;i++)
    {
        uint8_t offset = (7-i)*8;
        uint64_t mask = (static_cast<uint64_t>(0xFF) << offset);
        destination_buffer[index++] = static_cast<uint8_t>((value & mask) >> offset);
    }
    return index;
}

uint32_t ProbeMessagesEncoder::pack_uint64(ProbeMessagesEncoder::buffer_t *destination_buffer, uint32_t index, uint64_t *value, uint64_t n)
{
    uint32_t i,j;
    for(i=0,j=index;i<n;++i,j+=8) this->pack_uint64(destination_buffer,j,value[i]);
    return j;
}

uint64_t ProbeMessagesEncoder::unpack_uint64(ProbeMessagesEncoder::buffer_t **source_buffer)
{
    uint64_t value = 0;
    for(uint8_t i=0;i<8;i++)
    {
        uint8_t offset = (7-i)*8;
        uint64_t mask = (static_cast<uint64_t>(0xFF) << offset);
        value |= ((static_cast<uint64_t>(**source_buffer) << offset) & mask);
        (*source_buffer)++;
    }
    return value;
}

void ProbeMessagesEncoder::unpack_uint64(ProbeMessagesEncoder::buffer_t **source_buffer, uint64_t *destination_buffer, uint32_t n)
{
    for(uint32_t i=0;i<n;i++,destination_buffer++) *destination_buffer = this->unpack_uint64(source_buffer);
}

uint32_t ProbeMessagesEncoder::pack_frame_head_descriptor(ProbeMessagesEncoder::buffer_t *destination_buffer, ProbeMessagesEncoder::message::frame_head_descriptor_t &frame_head)
{
    destination_buffer[0] = static_cast<uint8_t>(frame_head.frame_id);
    uint32_t index = pack_uint32(destination_buffer,1,frame_head.dev_version);
    return pack_uint32(destination_buffer,index,frame_head.dev_unique_id);
}

uint32_t ProbeMessagesEncoder::pack_datetime(ProbeMessagesEncoder::buffer_t *destination_buffer, uint32_t index, ProbeMessagesEncoder::message::datetime_t &dtime)
{
    // WN
    destination_buffer[index++] = static_cast<buffer_t>((dtime.WN & 0x0FF0) >> 4);
    destination_buffer[index]   = static_cast<buffer_t>((dtime.WN & 0x000F) << 4);
    // TOW
    destination_buffer[index++] |= static_cast<buffer_t>((dtime.TOW & (0x0FUL << 16)) >> 16);
    destination_buffer[index++] = static_cast<buffer_t>((dtime.TOW & (0xFFUL << 8)) >> 8);
    destination_buffer[index++] = static_cast<buffer_t>(dtime.TOW & 0xFFUL);
    // TOW_F
    for(uint8_t shift = 32; shift != 0; shift -= 8)
    {
        destination_buffer[index] = static_cast<buffer_t>((dtime.TOW_F & (0x3FUL << shift)) >> (shift-2));
        destination_buffer[index++] |= static_cast<buffer_t>((dtime.TOW_F & (0xC0UL << (shift-8))) >> (shift-2));
    }
    destination_buffer[index] = static_cast<buffer_t>((dtime.TOW_F & 0x3FUL) << 2);
    // TOW_var
    for(uint8_t shift = 48; shift !=0; shift -= 8)
    {
        destination_buffer[index++] |= static_cast<buffer_t>((dtime.sygn_time_TOW_var & (0xC0UL << shift)) >> (shift+6));
        destination_buffer[index] = static_cast<buffer_t>((dtime.sygn_time_TOW_var & (0x3FUL << shift)) >> (shift-2));
    }
    destination_buffer[index++] |= static_cast<buffer_t>((dtime.sygn_time_TOW_var & 0xC0UL) >> 6);
    destination_buffer[index] = static_cast<buffer_t>((dtime.sygn_time_TOW_var & 0x3FUL) << 2);
    // status
    destination_buffer[index++] |= static_cast<buffer_t>((dtime.sync_status & 0x01) << 1);

    return index;
}

void ProbeMessagesEncoder::unpack_datetime(ProbeMessagesEncoder::buffer_t **source_buffer, message::datetime_t *dtime)
{
    // WN
    dtime->WN = static_cast<uint16_t>((**source_buffer) & 0xFF) << 4; (*source_buffer)++;
    dtime->WN |= static_cast<uint16_t>((**source_buffer) & 0xF0) >> 4;
    // TOW
    dtime->TOW = static_cast<uint32_t>((**source_buffer) & 0x0F) << 16; (*source_buffer)++;
    dtime->TOW |= static_cast<uint32_t>((**source_buffer) & 0xFF) << 8; (*source_buffer)++;
    dtime->TOW |= static_cast<uint32_t>((**source_buffer) & 0xFF); (*source_buffer)++;
    // TOW_F
    dtime->TOW_F = 0;
    for(uint8_t shift = 32; shift != 0; shift -= 8)
    {
        dtime->TOW_F |= static_cast<uint64_t>((**source_buffer) & 0xFF) << (shift-2); (*source_buffer)++;
    }
    dtime->TOW_F |= static_cast<uint64_t>((**source_buffer) & 0xFC) >> 2;
    // TOW_var
    dtime->sygn_time_TOW_var = 0;
    for(uint8_t shift = 48; shift !=0; shift -= 8)
    {
        dtime->sygn_time_TOW_var |= static_cast<uint64_t>((**source_buffer) & 0x03) << (shift+6); (*source_buffer)++;
        dtime->sygn_time_TOW_var |= static_cast<uint64_t>((**source_buffer) & 0xFC) << (shift-2);
    }
    dtime->sygn_time_TOW_var |= static_cast<uint64_t>((**source_buffer) & 0x03) << 6; (*source_buffer)++;
    dtime->sygn_time_TOW_var |= static_cast<uint64_t>((**source_buffer) & 0xFC) >> 2;
    // status
    dtime->sync_status = static_cast<uint8_t>((**source_buffer) & 0x02) >> 1; (*source_buffer)++;
}

uint32_t ProbeMessagesEncoder::pack_chirp_threat_descriptor(ProbeMessagesEncoder::buffer_t *destination_buffer, uint32_t index, messageThreat::chirp_threat_descriptor_t &dchirp)
{
    destination_buffer[index++] = static_cast<buffer_t>(dchirp.sygn_typ);
    index = pack_uint32(destination_buffer,index,static_cast<uint32_t>(dchirp.sygn_chirp_sweep));
    index = pack_uint32(destination_buffer,index,dchirp.sygn_chirp_period);
    index = pack_uint32(destination_buffer,index,dchirp.sygn_chirp_return);
    return pack_uint24(destination_buffer,index,dchirp.reserved);
}

void ProbeMessagesEncoder::unpack_chirp_threat_descriptor(ProbeMessagesEncoder::buffer_t **source_buffer, messageThreat::chirp_threat_descriptor_t *dchirp)
{
    dchirp->sygn_typ = static_cast<messageThreat::signal_type_t>(**source_buffer); (*source_buffer)++;
    dchirp->sygn_chirp_sweep = static_cast<int32_t>(this->unpack_uint32(source_buffer));
    dchirp->sygn_chirp_period = this->unpack_uint32(source_buffer);
    dchirp->sygn_chirp_return = this->unpack_uint32(source_buffer);
    dchirp->reserved = this->unpack_uint24(source_buffer);
}

uint32_t ProbeMessagesEncoder::pack_pulse_threat_descriptor(ProbeMessagesEncoder::buffer_t *destination_buffer, uint32_t index, messageThreat::pulse_threat_descriptor_t &dpulse)
{
    destination_buffer[index++] = static_cast<buffer_t>(dpulse.sygn_typ);
    index = pack_int16(destination_buffer,index,dpulse.sygn_pulse_p1);
    index = pack_int16(destination_buffer,index,dpulse.sygn_pulse_s1);
    index = pack_int16(destination_buffer,index,dpulse.sygn_pulse_p2);
    index = pack_int16(destination_buffer,index,dpulse.sygn_pulse_s2);
    index = pack_int16(destination_buffer,index,dpulse.sygn_pulse_p3);
    index = pack_int16(destination_buffer,index,dpulse.sygn_pulse_s3);
    index = pack_int16(destination_buffer,index,dpulse.sygn_pulse_p4);
    destination_buffer[index++] = static_cast<buffer_t>(dpulse.reserved);
    return index;
}

void ProbeMessagesEncoder::unpack_pulse_threat_descriptor(ProbeMessagesEncoder::buffer_t **source_buffer, messageThreat::pulse_threat_descriptor_t *dpulse)
{
    dpulse->sygn_typ = static_cast<messageThreat::signal_type_t>(**source_buffer); (*source_buffer)++;
    dpulse->sygn_pulse_p1 = unpack_int16(source_buffer);
    dpulse->sygn_pulse_s1 = unpack_int16(source_buffer);
    dpulse->sygn_pulse_p2 = unpack_int16(source_buffer);
    dpulse->sygn_pulse_s2 = unpack_int16(source_buffer);
    dpulse->sygn_pulse_p3 = unpack_int16(source_buffer);
    dpulse->sygn_pulse_s3 = unpack_int16(source_buffer);
    dpulse->sygn_pulse_p4 = unpack_int16(source_buffer);
    dpulse->reserved = **source_buffer; (*source_buffer)++;
}

uint32_t ProbeMessagesEncoder::pack_threat_descriptor(ProbeMessagesEncoder::buffer_t *destination_buffer, uint32_t index, messageThreat::threat_descriptor_t &dthreat)
{
    uint8_t head_size = sizeof(dthreat.head)/sizeof(dthreat.head[0]);
    memcpy(destination_buffer,dthreat.head,head_size); index += head_size;
    index = pack_datetime(destination_buffer,index,dthreat.sygn_time);
    index = pack_uint24(destination_buffer,index,dthreat.sygn_f0);
    index = pack_uint24(destination_buffer,index,dthreat.sygn_f0_var);
    index = pack_uint24(destination_buffer,index,dthreat.sygn_BW);
    index = pack_uint24(destination_buffer,index,dthreat.sygn_BW_var);
    index = pack_int16(destination_buffer,index,dthreat.sygn_No);
    index = pack_int16(destination_buffer,index,dthreat.sygn_J);
    if(dthreat.isChirped())
    {
        index = pack_chirp_threat_descriptor(destination_buffer,index,dthreat.sygn_data.chirp);
    }else
    {
        index = pack_pulse_threat_descriptor(destination_buffer,index,dthreat.sygn_data.pulse);
    }
    destination_buffer[index++] = dthreat.tail;

    return index;
}

void ProbeMessagesEncoder::unpack_threat_descriptor(ProbeMessagesEncoder::buffer_t **source_buffer, messageThreat::threat_descriptor_t *dthreat)
{
    uint8_t head_size = sizeof(dthreat->head)/sizeof(dthreat->head[0]);
    memcpy(dthreat->head,*source_buffer,head_size); (*source_buffer) += head_size;
    this->unpack_datetime(source_buffer,&dthreat->sygn_time);
    dthreat->sygn_f0           = this->unpack_uint24(source_buffer);
    dthreat->sygn_f0_var       = this->unpack_uint24(source_buffer);
    dthreat->sygn_BW           = this->unpack_uint24(source_buffer);
    dthreat->sygn_BW_var       = this->unpack_uint24(source_buffer);
    dthreat->sygn_No           = this->unpack_int16(source_buffer);
    dthreat->sygn_J            = this->unpack_int16(source_buffer);
    if(dthreat->getType() == messageThreat::MULTI_TONE)
    {
        this->unpack_pulse_threat_descriptor(source_buffer,&dthreat->sygn_data.pulse);
    }else
    {
        this->unpack_chirp_threat_descriptor(source_buffer,&dthreat->sygn_data.chirp);
    }
    dthreat->tail = **source_buffer; (*source_buffer)++;
}

uint32_t ProbeMessagesEncoder::pack_fft_descriptor(ProbeMessagesEncoder::buffer_t *destination_buffer, uint32_t index, messageFFT::fft_descriptor_t &dfft)
{
    destination_buffer[index++] = static_cast<buffer_t>(dfft.fft_f_begin);
    index = pack_int16(destination_buffer,index,static_cast<int16_t>(dfft.fft_sampling_rate));
    destination_buffer[index++] = static_cast<buffer_t>(dfft.fft_N_exponent);
    uint32_t N = (1 << dfft.fft_N_exponent);
    for(uint32_t i=0;i<N;++i)
    {
       destination_buffer[index++] = static_cast<buffer_t>(dfft.fft_power_density[i]);
    }
    return index;
}

void ProbeMessagesEncoder::unpack_fft_descriptor(ProbeMessagesEncoder::buffer_t **source_buffer, messageFFT::fft_descriptor_t *dfft)
{
    dfft->fft_f_begin = static_cast<uint8_t>(**source_buffer); (*source_buffer)++;
    dfft->fft_sampling_rate = unpack_int16(source_buffer);
    dfft->fft_N_exponent = static_cast<uint8_t>(**source_buffer); (*source_buffer)++;
    dfft->allocate(dfft->fft_N_exponent);
    uint32_t N = messageFFT::exponentToFFTlength(dfft->fft_N_exponent);
    for(uint32_t i=0;i<N;++i)
    {
        dfft->fft_power_density[i] = static_cast<int8_t>(**source_buffer);
        (*source_buffer)++;
    }
}

uint32_t ProbeMessagesEncoder::packHead(ProbeMessagesEncoder::buffer_t *destination_buffer, message::frame_type_t msg_type)
{
    message::frame_head_descriptor_t head;
    head.dev_unique_id = this->dev_unique_id;
    head.dev_version = this->dev_version;
    head.frame_id = msg_type;
    return this->pack_frame_head_descriptor(destination_buffer,head);
}

void ProbeMessagesEncoder::unpackHead(ProbeMessagesEncoder::buffer_t **source_buffer, message::frame_head_descriptor_t *dhead)
{
    dhead->frame_id = static_cast<message::frame_type_t>(**source_buffer); (*source_buffer)++;
    dhead->dev_version = this->unpack_uint32(source_buffer);
    dhead->dev_unique_id = this->unpack_uint32(source_buffer);
}

uint32_t ProbeMessagesEncoder::countBytesForThreatMessage()
{
    return messageThreat::threat_descriptor::getPackedSize();
}

uint32_t ProbeMessagesEncoder::countBytesForFFTMessage(messageFFT::fft_descriptor_t &dfft)
{
    uint32_t head_n = message::frame_head_descriptor::getPackedSize();
    uint32_t fft_n  = dfft.getPackedSize();
    return (head_n + fft_n);
}

ProbeMessagesEncoder::ProbeMessagesEncoder(uint32_t device_version, uint32_t device_unique_id):
dev_version(device_version),
dev_unique_id(device_unique_id)
{

}

ProbeMessagesEncoder::buffer_t *ProbeMessagesEncoder::packMessage(uint32_t *bytes_written, message *msg)
{
    *bytes_written = 0;
    buffer_t * buff = nullptr;
    if (msg->isEmpty()) return buff;

    switch (msg->getType())
    {
        case ProbeMessagesEncoder::message::FRAME_THREAT:
            {
                messageThreat * pThreat =  dynamic_cast<messageThreat*>(msg);
                if(!pThreat) break;
                buff = this->packThreatMessage(bytes_written,pThreat->getThreats());
                break;
            }
        case ProbeMessagesEncoder::message::FRAME_FFT:
            {
                messageFFT * pFFT =  dynamic_cast<messageFFT*>(msg);
                if(!pFFT) break;
                buff = this->packFFTMessage(bytes_written,pFFT->getFFT());
                break;
            }
        default:
            break;
    }
    return buff;
}

ProbeMessagesEncoder::msg_auto_ptr ProbeMessagesEncoder::unpackMessage(ProbeMessagesEncoder::buffer_t *buff, uint32_t length)
{
    if(length < HEAD_DESCRIPTOR_SIZE) return msg_auto_ptr();
    if(memcmp(buff,"#DIST",5) == 0){
        messageThreat* msgThreat = new messageThreat();
        msgThreat->threats_number = 1;
        msgThreat->threats_data = new messageThreat::threat_descriptor_t[1];
        this->unpack_threat_descriptor(&buff,&msgThreat->threats_data[0]);
        return static_cast<msg_auto_ptr>(msgThreat);
    }else {
        message::frame_head_descriptor_t head;
        this->unpackHead(&buff,&head);
        switch (head.frame_id) {
            case ProbeMessagesEncoder::message::FRAME_FFT:
                {
                    messageFFT* msgFFT = new messageFFT();
                    messageFFT::fft_descriptor_t fft;
                    msgFFT->head = head;
                    this->unpack_fft_descriptor(&buff,&fft);
                    msgFFT->setFFT(fft);
                    return static_cast<msg_auto_ptr>(msgFFT);
                }
                break;
            default:
                break;
        }
    }
    return msg_auto_ptr();
}

time_t TimeFromYMD(int year, int month, int day) {
  struct tm tm = {0};
  tm.tm_year = year - 1900;
  tm.tm_mon = month - 1;
  tm.tm_mday = day;
  return mktime(&tm);
}
#define SECS_PER_WEEK (60L*60*24*7)
time_t galileoBeginTime = TimeFromYMD(1998,8,21);


double ProbeMessagesEncoder::getGalileoSecondsSinceEpoch()
{
    auto time_now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(time_now);
    tm utc_tm_now = *gmtime(&tt);

    return difftime(tt, galileoBeginTime);
}

double ProbeMessagesEncoder::getGalileoSecondFraction()
{
    double galileo_time = ProbeMessagesEncoder::getGalileoSecondsSinceEpoch();
    auto int_time = static_cast<uint64_t>(galileo_time);
    return (galileo_time - static_cast<double>(int_time));
}

uint16_t ProbeMessagesEncoder::getGalileoWeekNumber()
{
    int wn = static_cast<int>(ProbeMessagesEncoder::getGalileoSecondsSinceEpoch() / SECS_PER_WEEK);
    // rollover
    wn = wn % 4096;
    return static_cast<uint16_t>(wn);
}

uint32_t ProbeMessagesEncoder::getGalileoSecondOfTheWeekNumber()
{
    double galileo_time = ProbeMessagesEncoder::getGalileoSecondsSinceEpoch();
    int wn = static_cast<int>(galileo_time / SECS_PER_WEEK);
    return (galileo_time - static_cast<double>(wn*SECS_PER_WEEK));
}

void ProbeMessagesEncoder::saveCurrentDateTime(ProbeMessagesEncoder::message::datetime_t *dest)
{
    dest->WN = ProbeMessagesEncoder::getGalileoWeekNumber();
    dest->TOW = ProbeMessagesEncoder::getGalileoSecondOfTheWeekNumber();
    dest->TOW_F = static_cast<uint64_t>(ProbeMessagesEncoder::getGalileoSecondFraction()*SCALE_SECOND_TO_TOW_F);
}

ProbeMessagesEncoder::buffer_t* ProbeMessagesEncoder::packThreatMessage(uint32_t *bytes_written, messageThreat::threat_descriptor_t *dthreat)
{
    // allocate memory for messages
    *bytes_written = this->countBytesForThreatMessage();
    buffer_t* destination_buffer = new buffer_t[*bytes_written];

    // pack threats data
    memcpy(dthreat->head,"#DIST",sizeof(dthreat->head)/sizeof(dthreat->head[0]));
    dthreat->tail = 'K';
    pack_threat_descriptor(destination_buffer,0,*dthreat);

    return destination_buffer;
}

ProbeMessagesEncoder::buffer_t* ProbeMessagesEncoder::packFFTMessage(uint32_t *bytes_written, messageFFT::fft_descriptor_t &dfft)
{
    // allocate memory for message
    *bytes_written = this->countBytesForFFTMessage(dfft);
    buffer_t* destination_buffer = new buffer_t[*bytes_written];
    // pack head
    uint32_t index = this->packHead(destination_buffer,ProbeMessagesEncoder::message::FRAME_FFT);
    index = pack_fft_descriptor(destination_buffer,index,dfft);
    return destination_buffer;
}

ProbeMessagesEncoder::messageThreat::variant_threat_data::variant_threat_data()
{
    chirp.sygn_typ = UNKNOWN;
}
