#include "probeMessagesEncoderTest.h"
#include <algorithm>
#include <cstring>

CPPUNIT_TEST_SUITE_REGISTRATION(probeMessagesEncoderTest);


void probeMessagesEncoderTest::setUp()
{
    this->encoder = new ProbeMessagesEncoder(this->dev_version,this->dev_unique_id);
    this->buff = new buffer_t[1024];
}

void probeMessagesEncoderTest::tearDown()
{
    delete this->encoder;
    delete [] this->buff;
}

void probeMessagesEncoderTest::test_PackUnpack_int16_ShouldReturnProperValue()
{
    // TEST 1
    buffer_t*p = buff;

    int16_t lhs= 0xF5AA;
    encoder->pack_int16(p,0,lhs);
    int16_t rhs= encoder->unpack_int16(&p);
    CPPUNIT_ASSERT_EQUAL(lhs,rhs);

    // TEST 2
    int16_t lhs2[2];
    int16_t rhs2[2];
    lhs2[0] = 0x4455;
    lhs2[1] = 0xBeef;
    p = buff;
    encoder->pack_int16(p,0,lhs2,2);
    encoder->unpack_int16(&p,rhs2,2);
    CPPUNIT_ASSERT_EQUAL(lhs2[0],rhs2[0]);
    CPPUNIT_ASSERT_EQUAL(lhs2[1],rhs2[1]);
}

void probeMessagesEncoderTest::test_PackUnpack_uint24_ShouldReturnProperValue()
{
    // TEST 1
    buffer_t*p = buff;

    uint32_t lhs= 0x00203040;
    encoder->pack_uint24(p,0,lhs);
    uint32_t rhs= encoder->unpack_uint24(&p);
    CPPUNIT_ASSERT_EQUAL(lhs,rhs);

    // TEST 2
    uint32_t lhs2[2];
    uint32_t rhs2[2];
    lhs2[0] = 0x00553257;
    lhs2[1] = 0x00efA575;
    p = buff;
    encoder->pack_uint24(p,0,lhs2,2);
    encoder->unpack_uint24(&p,rhs2,2);
    CPPUNIT_ASSERT_EQUAL(lhs2[0],rhs2[0]);
    CPPUNIT_ASSERT_EQUAL(lhs2[1],rhs2[1]);
}

void probeMessagesEncoderTest::test_PackUnpack_uint32_ShouldReturnProperValue()
{
    // TEST 1
    buffer_t*p = buff;

    uint32_t lhs= 0x10203040;
    encoder->pack_uint32(p,0,lhs);
    uint32_t rhs= encoder->unpack_uint32(&p);
    CPPUNIT_ASSERT_EQUAL(lhs,rhs);

    // TEST 2
    uint32_t lhs2[2];
    uint32_t rhs2[2];
    lhs2[0] = 0x44553257;
    lhs2[1] = 0xBeefA575;
    p = buff;
    encoder->pack_uint32(p,0,lhs2,2);
    encoder->unpack_uint32(&p,rhs2,2);
    CPPUNIT_ASSERT_EQUAL(lhs2[0],rhs2[0]);
    CPPUNIT_ASSERT_EQUAL(lhs2[1],rhs2[1]);
}

void probeMessagesEncoderTest::test_PackUnpack_uint64_ShouldReturnProperValue()
{
    // TEST 1
    buffer_t*p = buff;

    uint64_t lhs= 0xF5AABBCCEE992233UL;
    encoder->pack_uint64(p,0,lhs);
    uint64_t rhs= encoder->unpack_uint64(&p);
    CPPUNIT_ASSERT_EQUAL(lhs,rhs);

    // TEST 2
    uint64_t lhs2[2];
    uint64_t rhs2[2];
    lhs2[0] = 0xF5AABBCCEE992233UL;
    lhs2[1] = 0x23193741012836FFUL;
    p = buff;
    encoder->pack_uint64(p,0,lhs2,2);
    encoder->unpack_uint64(&p,rhs2,2);
    CPPUNIT_ASSERT_EQUAL(lhs2[0],rhs2[0]);
    CPPUNIT_ASSERT_EQUAL(lhs2[1],rhs2[1]);
}


void probeMessagesEncoderTest::test_PackUnpack_datetime_ShouldReturnProperValue()
{
    ProbeMessagesEncoder::message::datetime_t lhs;
    ProbeMessagesEncoder::message::datetime_t rhs;
    buffer_t*p = buff;

    lhs = getTestDateTime();

    encoder->pack_datetime(p,0,lhs);
    encoder->unpack_datetime(&p,&rhs);

    CPPUNIT_ASSERT(lhs == rhs);
}

void probeMessagesEncoderTest::test_PackUnpack_chirp_threat_descriptor_ShouldReturnProperValue()
{
    ProbeMessagesEncoder::messageThreat::chirp_threat_descriptor_t lhs;
    ProbeMessagesEncoder::messageThreat::chirp_threat_descriptor_t rhs;
    buffer_t*p = buff;

    lhs = this->getTestChirp();

    encoder->pack_chirp_threat_descriptor(p,0,lhs);
    encoder->unpack_chirp_threat_descriptor(&p,&rhs);

    CPPUNIT_ASSERT(lhs == rhs);
}

void probeMessagesEncoderTest::test_PackUnpack_pulse_threat_descriptor_ShouldReturnProperValue()
{
    ProbeMessagesEncoder::messageThreat::pulse_threat_descriptor_t lhs;
    ProbeMessagesEncoder::messageThreat::pulse_threat_descriptor_t rhs;
    buffer_t*p = buff;

    lhs = getTestPulse();

    encoder->pack_pulse_threat_descriptor(p,0,lhs);
    encoder->unpack_pulse_threat_descriptor(&p,&rhs);

    CPPUNIT_ASSERT(lhs == rhs);
}

void probeMessagesEncoderTest::test_PackUnpack_threat_descriptor_ShouldReturnProperValue()
{
    ProbeMessagesEncoder::messageThreat::threat_descriptor_t lhs;
    ProbeMessagesEncoder::messageThreat::threat_descriptor_t rhs;
    buffer_t*p = buff;

    // TEST 1
    std::memcpy(lhs.head,"#DIST",5);
    lhs.sygn_BW             = 0x00345678;
    lhs.sygn_BW_var         = 0x00579120;
    lhs.sygn_f0             = 0x00904156;
    lhs.sygn_f0_var         = 0x00571004;
    lhs.sygn_J              = 0xFF52;
    lhs.sygn_No             = 0x8942;
    lhs.sygn_time           = this->getTestDateTime();
    lhs.sygn_data.chirp     = this->getTestChirp();
    lhs.tail = 'k';

    encoder->pack_threat_descriptor(p,0,lhs);
    encoder->unpack_threat_descriptor(&p,&rhs);

    CPPUNIT_ASSERT(lhs == rhs);

    // TEST 2
    lhs.sygn_data.pulse     = this->getTestPulse();
    p = buff;

    encoder->pack_threat_descriptor(p,0,lhs);
    encoder->unpack_threat_descriptor(&p,&rhs);

    CPPUNIT_ASSERT(lhs == rhs);
}

void probeMessagesEncoderTest::test_PackUnpack_fft_descriptor_ShouldReturnProperValue()
{
    ProbeMessagesEncoder::messageFFT::fft_descriptor_t rhs;
    ProbeMessagesEncoder::messageFFT::fft_descriptor_t lhs;
    buffer_t*p = buff;

    lhs = getTestFFT(7);

    encoder->pack_fft_descriptor(p,0,lhs);
    encoder->unpack_fft_descriptor(&p,&rhs);

    CPPUNIT_ASSERT(lhs == rhs);
}

void probeMessagesEncoderTest::test_PackUnpack_head_ShouldReturnProperValue()
{
    ProbeMessagesEncoder::message::frame_head_descriptor_t rhs;
    ProbeMessagesEncoder::message::frame_head_descriptor_t lhs;
    buffer_t*p = buff;

    rhs.frame_id = ProbeMessagesEncoder::message::FRAME_FFT;
    rhs.dev_unique_id = this->dev_unique_id;
    rhs.dev_version = this->dev_version;

    encoder->packHead(p,ProbeMessagesEncoder::message::FRAME_FFT);
    encoder->unpackHead(&p,&lhs);

    CPPUNIT_ASSERT_EQUAL(rhs.frame_id,lhs.frame_id);
    CPPUNIT_ASSERT_EQUAL(rhs.dev_unique_id,lhs.dev_unique_id);
    CPPUNIT_ASSERT_EQUAL(rhs.dev_version,lhs.dev_version);
}

void probeMessagesEncoderTest::test_PackUnpack_ThreatMessage_ShouldReturnEqualFrames()
{
    ProbeMessagesEncoder::messageThreat lhs;
    lhs.appendThreat(this->getTestThreat(ProbeMessagesEncoder::messageThreat::NARROW_SWEEP));

    uint32_t n;
    buffer_t* buffer = encoder->packMessage(&n,&lhs);
    ProbeMessagesEncoder::msg_auto_ptr decoder = encoder->unpackMessage(buffer,n);
    delete [] buffer;

    ProbeMessagesEncoder::messageThreat *rhs = dynamic_cast<ProbeMessagesEncoder::messageThreat*>(decoder.get());
    if(rhs)
    {
        CPPUNIT_ASSERT(lhs == *rhs);
    } else
    {
        CPPUNIT_ASSERT(false);
    }
}

void probeMessagesEncoderTest::test_PackUnpack_FFTMessage_ShouldReturnEqualFrames()
{

    ProbeMessagesEncoder::messageFFT lhs;
    // TEST 1
    lhs.getHead().dev_unique_id = this->dev_unique_id;
    lhs.getHead().dev_version = this->dev_version;
    lhs.setFFT(this->getTestFFT(7));

    uint32_t n;
    buffer_t* buffer = encoder->packMessage(&n,&lhs);
    ProbeMessagesEncoder::msg_auto_ptr decoder = encoder->unpackMessage(buffer,n);
    delete [] buffer;

    ProbeMessagesEncoder::messageFFT * rhs;
    rhs = dynamic_cast<ProbeMessagesEncoder::messageFFT*>(decoder.get());
    CPPUNIT_ASSERT(rhs != nullptr);
    if(rhs)
    {
        CPPUNIT_ASSERT(lhs == *rhs);
    }
    // TEST 2
    lhs.getHead().dev_unique_id = this->dev_unique_id;
    lhs.getHead().dev_version = this->dev_version;
    lhs.setFFT(this->getTestFFT(8));

    buffer = encoder->packMessage(&n,&lhs);
    ProbeMessagesEncoder::msg_auto_ptr decoder2 = encoder->unpackMessage(buffer,n);
    delete [] buffer;

    ProbeMessagesEncoder::messageFFT* rhs2 = dynamic_cast<ProbeMessagesEncoder::messageFFT*>(decoder2.get());
    CPPUNIT_ASSERT(rhs2 != nullptr);
    if(rhs)
    {
        CPPUNIT_ASSERT(lhs == *rhs2);
    }

}

void probeMessagesEncoderTest::test_threat_descriptor_getPackedSize_ShouldReturnProperValue()
{
    ProbeMessagesEncoder::messageThreat::threat_descriptor_t lhs;
    ProbeMessagesEncoder::messageThreat::threat_descriptor_t rhs;
    buffer_t*p = buff;

    // TEST 1
    std::memcpy(lhs.head,"#DIST",5);
    lhs.sygn_BW             = 0x00345678;
    lhs.sygn_BW_var         = 0x00579120;
    lhs.sygn_f0             = 0x00904156;
    lhs.sygn_f0_var         = 0x00571004;
    lhs.sygn_J              = 0xFF52;
    lhs.sygn_No             = 0x8942;
    lhs.sygn_time           = this->getTestDateTime();
    lhs.sygn_data.chirp     = this->getTestChirp();
    lhs.tail = 'k';

    auto bytes_written = encoder->pack_threat_descriptor(p,0,lhs);


    CPPUNIT_ASSERT_EQUAL(bytes_written,lhs.getPackedSize());
}

void probeMessagesEncoderTest::test_packMessage_ThreatMessage_ShouldReturnProperBufferLength()
{
    ProbeMessagesEncoder::messageThreat lhs;
    auto threat = this->getTestThreat(ProbeMessagesEncoder::messageThreat::NARROW_SWEEP);
    lhs.appendThreat(threat);

    uint32_t n;
    buffer_t* buffer = encoder->packMessage(&n,&lhs);
    delete [] buffer;

    CPPUNIT_ASSERT_EQUAL(n,threat.getPackedSize());
}

ProbeMessagesEncoder::message::datetime_t probeMessagesEncoderTest::getTestDateTime()
{
    ProbeMessagesEncoder::message::datetime_t lhs;
    lhs.sygn_time_TOW_var = 0x00F3156825109FADUL;
    lhs.TOW_F = 0x3FABCDEF98UL;
    lhs.TOW  = 0x0FEBAC;
    lhs.WN = 0x0FDC;
    lhs.sync_status = 1;
    return lhs;
}

ProbeMessagesEncoder::messageThreat::chirp_threat_descriptor_t probeMessagesEncoderTest::getTestChirp()
{
    ProbeMessagesEncoder::messageThreat::chirp_threat_descriptor_t lhs;
    lhs.sygn_typ = ProbeMessagesEncoder::messageThreat::WIDE_SWEEP_MEDIUM;
    lhs.sygn_chirp_sweep = 0xFFAABB44;
    lhs.sygn_chirp_period = 0x55113399;
    lhs.sygn_chirp_return = 0x44228513;
    lhs.reserved = 0x00FEDCBA;
    return lhs;
}

ProbeMessagesEncoder::messageThreat::pulse_threat_descriptor_t probeMessagesEncoderTest::getTestPulse()
{
    ProbeMessagesEncoder::messageThreat::pulse_threat_descriptor_t lhs;

    lhs.sygn_typ = ProbeMessagesEncoder::messageThreat::MULTI_TONE;
    lhs.sygn_pulse_p1 = 0xBB44;
    lhs.sygn_pulse_p2 = 0xFA59;
    lhs.sygn_pulse_p3 = 0x04CA;
    lhs.sygn_pulse_p4 = 0x2312;
    lhs.sygn_pulse_s1 = 0x1234;
    lhs.sygn_pulse_s2 = 0x5678;
    lhs.sygn_pulse_s3 = 0x9ABC;
    lhs.reserved = 0xAF;

    return lhs;
}

ProbeMessagesEncoder::messageThreat::threat_descriptor_t probeMessagesEncoderTest::getTestThreat(ProbeMessagesEncoder::messageThreat::signal_type_t type)
{
    ProbeMessagesEncoder::messageThreat::threat_descriptor_t rhs;

    rhs.sygn_BW             = 0x00345678;
    rhs.sygn_BW_var         = 0x00579120;
    rhs.sygn_f0             = 0x00904156;
    rhs.sygn_f0_var         = 0x00571004;
    rhs.sygn_J              = 0xFF52;
    rhs.sygn_No             = 0x8942;
    rhs.sygn_time           = this->getTestDateTime();
    if(type == ProbeMessagesEncoder::messageThreat::MULTI_TONE)
    {
        rhs.sygn_data.pulse = this->getTestPulse();
        rhs.sygn_data.pulse.sygn_typ = type;
    }else
    {
        rhs.sygn_data.chirp = this->getTestChirp();
        rhs.sygn_data.chirp.sygn_typ = type;
    }


    return rhs;
}

ProbeMessagesEncoder::messageFFT::fft_descriptor_t probeMessagesEncoderTest::getTestFFT(uint8_t N_exponent)
{
    ProbeMessagesEncoder::messageFFT::fft_descriptor_t lhs;

    lhs.fft_f_begin    = 0x67;
    lhs.fft_sampling_rate = 0x2395;
    lhs.allocate(N_exponent);
    uint8_t val = 77;
    uint32_t N = lhs.count();
    for(uint32_t i=0;i<N;i++)
    {
        lhs.fft_power_density[i] = val = (val<<1) | (val>>(8-1));
    }
    return lhs;
}

