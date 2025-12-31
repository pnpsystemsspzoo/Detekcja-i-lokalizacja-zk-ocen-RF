#ifndef PROBEMESSAGESENCODERTEST_H
#define PROBEMESSAGESENCODERTEST_H

#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <netinet/in.h>

#include "../sources/probeMessagesEncoder.h"

using namespace CppUnit;
using namespace std;

using buffer_t = ProbeMessagesEncoder::buffer_t;

class probeMessagesEncoderTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(probeMessagesEncoderTest);
    CPPUNIT_TEST(test_PackUnpack_int16_ShouldReturnProperValue);
    CPPUNIT_TEST(test_PackUnpack_uint24_ShouldReturnProperValue);
    CPPUNIT_TEST(test_PackUnpack_uint32_ShouldReturnProperValue);
    CPPUNIT_TEST(test_PackUnpack_uint64_ShouldReturnProperValue);
    CPPUNIT_TEST(test_PackUnpack_datetime_ShouldReturnProperValue);
    CPPUNIT_TEST(test_PackUnpack_chirp_threat_descriptor_ShouldReturnProperValue);
    CPPUNIT_TEST(test_PackUnpack_pulse_threat_descriptor_ShouldReturnProperValue);
    CPPUNIT_TEST(test_PackUnpack_threat_descriptor_ShouldReturnProperValue);
    CPPUNIT_TEST(test_PackUnpack_fft_descriptor_ShouldReturnProperValue);
    CPPUNIT_TEST(test_PackUnpack_head_ShouldReturnProperValue);
    CPPUNIT_TEST(test_PackUnpack_ThreatMessage_ShouldReturnEqualFrames);
    CPPUNIT_TEST(test_PackUnpack_FFTMessage_ShouldReturnEqualFrames);
    CPPUNIT_TEST(test_threat_descriptor_getPackedSize_ShouldReturnProperValue);
    CPPUNIT_TEST(test_packMessage_ThreatMessage_ShouldReturnProperBufferLength);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:

    void test_PackUnpack_int16_ShouldReturnProperValue();
    void test_PackUnpack_uint24_ShouldReturnProperValue();
    void test_PackUnpack_uint32_ShouldReturnProperValue();
    void test_PackUnpack_uint64_ShouldReturnProperValue();
    void test_PackUnpack_datetime_ShouldReturnProperValue();
    void test_PackUnpack_chirp_threat_descriptor_ShouldReturnProperValue();
    void test_PackUnpack_pulse_threat_descriptor_ShouldReturnProperValue();
    void test_PackUnpack_threat_descriptor_ShouldReturnProperValue();
    void test_PackUnpack_fft_descriptor_ShouldReturnProperValue();
    void test_PackUnpack_head_ShouldReturnProperValue();
    void test_PackUnpack_ThreatMessage_ShouldReturnEqualFrames();
    void test_PackUnpack_FFTMessage_ShouldReturnEqualFrames();

    void test_threat_descriptor_getPackedSize_ShouldReturnProperValue();
    void test_packMessage_ThreatMessage_ShouldReturnProperBufferLength();

private:

    uint32_t dev_version = 10;
    uint32_t dev_unique_id = 2;

    ProbeMessagesEncoder * encoder;
    buffer_t * buff;

    ProbeMessagesEncoder::message::datetime_t getTestDateTime();
    ProbeMessagesEncoder::messageThreat::chirp_threat_descriptor_t getTestChirp();
    ProbeMessagesEncoder::messageThreat::pulse_threat_descriptor_t getTestPulse();
    ProbeMessagesEncoder::messageThreat::threat_descriptor_t getTestThreat(ProbeMessagesEncoder::messageThreat::signal_type_t type);
    ProbeMessagesEncoder::messageFFT::fft_descriptor_t getTestFFT(uint8_t N_exponent);
};


#endif // PROBEMESSAGESENCODERTEST_H
