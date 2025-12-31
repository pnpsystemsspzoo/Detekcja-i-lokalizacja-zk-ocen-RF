/** @file */

#ifndef PROBEMESSAGESENCODER_H
#define PROBEMESSAGESENCODER_H

#include <stdint.h>
#include <memory>
#include <auto_ptr.h>

/*! @brief predefined maximum number of threats in single threat message */
#define MAX_THREATS_IN_SINGLE_MESSAGE 1
/*!
 *  @brief predefined frame head descriptor size in packed message
 *  @warning This is not a paramater. This value should not be changed
 *  without code modification */
#define HEAD_DESCRIPTOR_SIZE 9
/*!
 *  @brief conversion scale factor from seconds to the TOW_F LSB from ProbeMessageEncoder::message::datetime_t
 *  @warning This is not a paramater. This value should not be changed
 *  without code modification */
#define SCALE_SECOND_TO_TOW_F (1/(4E-12))
/*!
 * @brief Class to encode/decode probe sent messages
 * This class packs probe messages into bytes buffer
 * that are ready to be sent through TCP. It also performs
 * decoding
 */
class ProbeMessagesEncoder
{
    friend class probeMessagesEncoderTest;

public:

    using buffer_t = char; ///< packed buffer alias
    /*!
     * @brief Container class to hold probe messages descriptors
     * This is the base class for all probe messages being sent
     * to the central server. It describes frame head -> a common
     * fields for all messages */
    class message
    {
        friend class ProbeMessagesEncoder;

        public:
            typedef enum
            {
                FRAME_RESERVED = 0, ///< Reserved value, do not use
                FRAME_FFT = 1,      ///< Fourier transform message
                FRAME_THREAT = 2    ///< Threat message
            }frame_type_t; ///< Frame identifier enumerated type
            typedef struct frame_head_descriptor
            {
                frame_type_t frame_id;  ///< Frame identifier, holds frame type
                uint32_t dev_version;   ///< Encoded device version
                uint32_t dev_unique_id; ///< Device unique id to distinguish between multiple probes
                /*!
                 * @brief gets head descriptor packed size in bytes
                 * @return size of the head descriptor in packed buffer
                 */
                static uint32_t getPackedSize() {return HEAD_DESCRIPTOR_SIZE;}
            }frame_head_descriptor_t; ///< Frame head descriptor type
            typedef struct datetime
            {
                uint16_t WN; ///< Week number from 21.08.1999, 23:59:43
                uint32_t TOW; ///< Second of the week
                uint64_t TOW_F; ///< Second fraction
                uint64_t sygn_time_TOW_var; ///< TOW variance
                uint8_t sync_status; ///< Time synchronization status. 1:time synchronized, 0:time not synchronized
                /*!
                 * @brief equality operator
                 * @param other, an other datetime value to compare with
                 * @return true if all struct fields are the same. False otherwise
                 */
                bool operator ==(const datetime & other);
                /*!
                 * @brief gets datetime descriptor packed size in bytes
                 * @return size of the datetime descriptor in packed buffer
                 */
                static uint32_t getPackedSize() {return 16;}
            }datetime_t; ///< date and time descriptor
        private:
            frame_head_descriptor head; ///< message head descriptor
        public:
            /*!
             * @brief getter for frame head
             * @return reference to frame head descriptor
             */
            frame_head_descriptor_t & getHead() {return this->head;}
            /*!
             * @brief getter for frame id
             * @return frame identifier
             */
            frame_type_t getType() {return this->getHead().frame_id;}
            /*!
             * @brief getter for device unique id
             * @return device unique id
             */
            uint32_t getDeviceUniqueId(){return this->getHead().dev_unique_id;}
            /*!
             * @brief getter for device version
             * @return returns encoded device version
             */
            uint32_t getDeviceVersion(){return this->getHead().dev_version;}
            /*!
             * @brief Constructor
             * @param type, a specified frame id
             */
            message(frame_type_t type=FRAME_RESERVED);
            /*!
             * @brief Checks if message holds any data aside from frame head
             * @return True if message contains any data. False otherwise
             */
            virtual bool isEmpty() const {return true;}
            /*!
             * @brief Checks if messages are the same
             * @param rhs, right-hand-side message to compare with
             * @return True if messages are the same. False if any message field differs
             */
            virtual bool operator==(const message& rhs);
            /*!
             * @brief Virtual destructor
             */
            virtual ~message();
    };
    using msg_auto_ptr = std::auto_ptr<ProbeMessagesEncoder::message>; ///< alias for message std::auto_ptr
    /*!
     * @brief This is the container class for threat message sent from probe to the central server
     */
    class messageThreat : public message
    {
        friend class ProbeMessagesEncoder;

    public:
        typedef enum SIGNAL_TYPE
        {
            UNKNOWN             = 0, ///< Any not recognized type
            WIDE_SWEEP_SLOW     = 1,
            WIDE_SWEEP_MEDIUM   = 2,
            WIDE_SWEEP_FAST     = 3,
            WIDE_SWEEP_RAPID    = 4,
            NARROW_SWEEP        = 5,
            TRIANGULAR_WAVE     = 6,
            TRIANGULAR          = 7,
            SAWTOOTH            = 8,
            HOOKED_SAWTOOTH     = 9,
            TICK                =10,
            MULTI_TONE          =11,
            SIGNAL_TYPE_RESERVED ///< Reserved value, keep this value always at the end of enumerator
        }signal_type_t; ///< Enumerator for threat signal type
        typedef struct chirp_threat_descriptor
        {
            signal_type_t sygn_typ; ///< recognized signal type
            int32_t sygn_chirp_sweep;   ///< signal sweep rate
            uint32_t sygn_chirp_period; ///< signal period
            uint32_t sygn_chirp_return; ///< signal return time
            uint32_t reserved; ///< reserved
            /*!
             * @brief Equality operator
             * @param other, an other chirp threat descritpor to compare with
             * @return True if all fields are the same. False otherwise
             */
            bool operator ==(const chirp_threat_descriptor & other);
            /*!
             * @brief Inequality operator
             * @param other, an other chirp threat descriptor to compare with
             * @return True if any of the field differs. False otherwise
             */
            bool operator !=(const chirp_threat_descriptor & other) {return !(*this ==(other));}
        }chirp_threat_descriptor_t; ///< The descriptor with chirp like threats parameters
        typedef struct pulse_threat_descriptor
        {
            signal_type_t sygn_typ; ///< recognized signal type
            uint16_t sygn_pulse_p1; ///< Delta t between timestamp and first pulse
            uint16_t sygn_pulse_s1; ///< First pulse duration
            uint16_t sygn_pulse_p2; ///< Delta t between first and second pulse
            uint16_t sygn_pulse_s2; ///< Second pulse duration
            uint16_t sygn_pulse_p3; ///< Delta t between second and third pulse
            uint16_t sygn_pulse_s3; ///< Third pulse duration
            uint16_t sygn_pulse_p4; ///< Delta t between third and fourth pulse
            uint8_t reserved; ///< reserved field
            /*!
             * @brief Equality operator
             * @param other, an other pulse threat descriptor to compare with
             * @return True if all fields are the same. False otherwise
             */
            bool operator ==(const pulse_threat_descriptor & other);
            /*!
             * @brief Inequality operator
             * @param other, an other pulse threat descriptor to compare with
             * @return True if any of the field differs. False otherwise
             */
            bool operator !=(const pulse_threat_descriptor & other) {return !(*this ==(other));}
        }pulse_threat_descriptor_t;  ///< The descriptor with pulse like threats parameters
        typedef union variant_threat_data
        {
            chirp_threat_descriptor_t chirp; ///< chirp like threat specific parameters
            pulse_threat_descriptor_t pulse; ///< pulse like threat specific parameters
            /*!
             * @brief Default constructor
             */
            variant_threat_data();
        }variant_threat_data_t; ///< Union for threat specific data fields. Can be chirp like or pulse like.
        typedef struct threat_descriptor
        {
            char head[5]; ///< Threat descriptor head
            datetime_t sygn_time; ///< encoded time and date of the message
            uint32_t sygn_f0; ///< central frequency
            uint32_t sygn_f0_var; ///< central frequency variance
            uint32_t sygn_BW; ///< bandwidth
            uint32_t sygn_BW_var; ///< bandwidth variance
            int16_t sygn_No; ///< noise power
            int16_t sygn_J; ///< signal power
            variant_threat_data_t sygn_data; ///< threat specific data fields
            char tail; ///< Threat descriptor tail
            /*!
             * @brief gets threat descriptor packed size in bytes
             * @return size of the threat descriptor in packed buffer
             */
            static uint32_t getPackedSize() {return 5+message::datetime::getPackedSize()+32+1;}
            /*!
             * @brief Equality operator
             * @param other, an other threat descriptor to compare with
             * @return True if all fields are the same. False otherwise
             */
            bool operator ==(const threat_descriptor & other);
            /*!
             * @brief Returns threat signal type
             * @return signal_type_t, a threat signal type
             */
            signal_type_t getType() const {return this->sygn_data.chirp.sygn_typ;}
            /*!
             * @brief Checks if threat signal is some kind of chirp type
             * @return true if threat signal is some kind of chirp type. Otherwise false
             */
            bool isChirped() const {return this->getType() != MULTI_TONE ? true : false;}
            /*!
             * @brief Inequality operator
             * @param other, an other threat descriptor to compare with
             * @return True if any of the field differs. False otherwise
             */
            bool operator !=(const threat_descriptor & other) {return !(*this ==(other));}
        }threat_descriptor_t; ///< Threat descriptor
    private:
        uint8_t threats_number; ///< number of threats that this message stores
        threat_descriptor_t * threats_data; ///< pointer to threats array this message stores
    public:
        /*!
         * @brief Default constructor
         */
        messageThreat();
        /*!
         * @brief Destructor
         */
        ~messageThreat();
        /*!
         * @brief counts threats stored by message
         * @return number of threats saved in message
         */
        uint8_t countThreats() const {return threats_number;}
        /*!
         * @brief gets array of the threats stored in message
         * Check array size by calling countThreats()
         * @return Array of threats descriptors
         */
        threat_descriptor_t * getThreats() const {return threats_data;}
        /*!
         * @brief checks if message contains any threat data
         * @return True if there is at least one threat descriptor saved. False otherwise
         */
        bool isEmpty() const {return (this->countThreats() == 0);}
        /*!
         * @brief Equality operator
         * @param other, an other threat message to compare with
         * @return True if all message fields are the same. False otherwise
         */
        bool operator==(const messageThreat& rhs);
        /*!
         * @brief Copies threat descriptor to the message if there is space left in the message
         * @param threat, a reference to the threat descriptor which should be copied to the message
         * @return True if successfully descriptor was successfully copied into message. False if there is no space left (message already contains maximum number of threats)
         */
        bool appendThreat(const threat_descriptor_t &threat);
        /*!
         * @brief Checks if message contains maximum allowed threat numbers (is full)
         * @return True if message is full and no other threats can be copied into message. False otherwise
         */
        bool isFull() const {return countThreats() == MAX_THREATS_IN_SINGLE_MESSAGE;}
    };
    /*!
     * @brief This is the container class for FFT message sent from probe to the central server
     */
    class messageFFT : public message
    {
        friend class ProbeMessagesEncoder;

    public:
        typedef struct fft_descriptor
        {
            uint8_t fft_f_begin; ///< FFT begining frequency
            uint16_t fft_sampling_rate; ///< FFT bandwitdh
            uint8_t fft_N_exponent; ///< exponent x for the number of samples N=2^x
            int8_t *fft_power_density=nullptr; ///< array of the power densities
            /*!
             * @brief Preallocates fft_power_density buffer
             * @param exponent, an exponent x such that number of samples N (buffer size) in FFT equals N=2^x
             */
            void allocate(uint8_t exponent);
            /*!
             * @brief Releases dynamically allocated power densities array
             */
            void clearBuffer();
            /*!
             * @brief Default constructor
             */
            fft_descriptor();
            /*!
             * @brief Desctructor
             */
            ~fft_descriptor();
            /*!
             * @brief Equality operator
             * @param other, an other fft descriptor to compare with
             * @return True if all descriptor fields are the same. False otherwise
             */
            bool operator ==(const fft_descriptor & other);
            /*!
             * @brief Inequality operator
             * @param other, an other fft descriptor to compare with
             * @return True if any of the field differs. False otherwise
             */
            bool operator !=(const fft_descriptor & other) {return !(*this ==(other));}
            /*!
             * @brief Copy constructor
             * @param other, an other descriptor from which data should be copied
             */
            fft_descriptor(const fft_descriptor& other);
            /*!
             * @brief Assigment operator
             * @param other, an other descriptor from which data should be copied
             */
            void operator =(const fft_descriptor& other);
            /*!
             * @brief Counts FFT samples
             * @return number of samples in the FFT
             */
            uint32_t count() const {return exponentToFFTlength(this->fft_N_exponent);}
            /*!
             * @brief Gets fft descriptor packed size in bytes
             * @return size of the fft descriptor in packed buffer
             */
            uint32_t getPackedSize() {return (count()+4);}
        }fft_descriptor_t; ///< FFT data descriptor
    private:
        fft_descriptor_t * fft_data=nullptr; ///< pointer to the FFT data descriptor
        /*!
         * @brief Retrives FFT length from exponent value
         * It performs 2^x calculation
         * @param exponent, an exponent x of the base 2 which is N=2^x where N is number of FFT samples
         * @return number of samples in FFT
         */
        static uint32_t exponentToFFTlength(uint8_t exponent) {return (1<<exponent);}
    public:
        /*!
         * @brief Gets FFT descriptor from message
         * @return FFT descriptor that is hold by message
         */
        fft_descriptor_t & getFFT(){return *fft_data;}
        /*!
         * @brief Sets FFT descriptor for message
         * @param data, a reference to the FFT descriptor that will be copied into message
         */
        void setFFT(const fft_descriptor_t &data);
        /*!
         * @brief Checks if message contains FFT data
         * @return True if message do not hold any FFT data. Otherwise False
         */
        bool isEmpty() const {return fft_data == nullptr;}
        /*!
         * @brief Clears FFT data inside message and releases its internal buffer
         */
        void clear();
        /*!
         * @brief Default constructor
         */
        messageFFT();
        /*!
         * @brief Destructor
         */
        ~messageFFT();
    };


    uint32_t dev_version; ///< Device version. This value will always be encoded in message head.
    uint32_t dev_unique_id; ///< Device unique id. This value will always be encoded in message head.

    /*!
     * @brief Packs int16_t value to the output buffer and preserves big endian notation
     * @param destination_buffer, pointer to the buffer where data should be written
     * @param index, offset position where to start writing data in the destination_buffer
     * @param value, int16_t value that should be written to the buffer at specified position
     * @return incremented index value by size of the written data. This is the offset position where next data can be written.
     */
    uint32_t pack_int16(buffer_t*destination_buffer,uint32_t index,int16_t value);
    /*!
     * @brief Packs int16_t array to the output buffer and preserves big endian notation
     * @param destination_buffer, pointer to the buffer where data should be written
     * @param index, offset position where to start writing data in the destination_buffer
     * @param values, source buffer that should be written to the destination_buffer at specified position
     * @param n, number of items in source buffer
     * @return incremented index value by size of the written data. This is the offset position where next data can be written.
     */
    uint32_t pack_int16(buffer_t*destination_buffer,uint32_t index,int16_t *values,uint32_t n);
    /*!
     * @brief Unpacks int16_t from given buffer with big endian notation taken into account and increments data pointer by size of the read bytes
     * @param source_buffer, double pointer (pointer to the pointer) to the source buffer from where data should be retrived.
     * @return decoded int16_t value
     */
    int16_t unpack_int16(buffer_t**source_buffer);
    /*!
     * @brief Unpacks int16_t array from given buffer with big endian notation taken into account and increments data pointer by size of the read bytes
     * @param source_buffer, double pointer (pointer to the pointer) to the source buffer from where data should be retrived.
     * @param destination_buffer, a pointer to the buffer where decoded data should be written
     * @param n, number of items to decode
     */
    void unpack_int16(buffer_t**source_buffer,int16_t*destination_buffer,uint32_t n);
    /*!
     * @brief Packs uint24_t value to the output buffer and preserves big endian notation
     * @param destination_buffer, pointer to the buffer where data should be written
     * @param index, offset position where to start writing data in the destination_buffer
     * @param value, uint24_t value that should be written to the buffer at specified position
     * @return incremented index value by size of the written data. This is the offset position where next data can be written.
     */
    uint32_t pack_uint24(buffer_t*destination_buffer, uint32_t index, uint32_t value);
    /*!
     * @brief Packs uint24_t array to the output buffer and preserves big endian notation
     * @param destination_buffer, pointer to the buffer where data should be written
     * @param index, offset position where to start writing data in the destination_buffer
     * @param values, source buffer that should be written to the destination_buffer at specified position
     * @param n, number of items in source buffer
     * @return incremented index value by size of the written data. This is the offset position where next data can be written.
     */
    uint32_t pack_uint24(buffer_t*destination_buffer, uint32_t index, uint32_t *values, uint32_t n);
    /*!
     * @brief Unpacks uint24_t from given buffer with big endian notation taken into account and increments data pointer by size of the read bytes
     * @param source_buffer, double pointer (pointer to the pointer) to the source buffer from where data should be retrived.
     * @return decoded uint24_t value
     */
    uint32_t unpack_uint24(buffer_t**source_buffer);
    /*!
     * @brief Unpacks uint24_t array from given buffer with big endian notation taken into account and increments data pointer by size of the read bytes
     * @param source_buffer, double pointer (pointer to the pointer) to the source buffer from where data should be retrived.
     * @param destination_buffer, a pointer to the buffer where decoded data should be written
     * @param n, number of items to decode
     */
    void unpack_uint24(buffer_t**source_buffer,uint32_t*destination_buffer,uint32_t n);
    /*!
     * @brief Packs uint32_t value to the output buffer and preserves big endian notation
     * @param destination_buffer, pointer to the buffer where data should be written
     * @param index, offset position where to start writing data in the destination_buffer
     * @param value, uint32_t value that should be written to the buffer at specified position
     * @return incremented index value by size of the written data. This is the offset position where next data can be written.
     */
    uint32_t pack_uint32(buffer_t*destination_buffer, uint32_t index, uint32_t value);
    /*!
     * @brief Packs uint32_t array to the output buffer and preserves big endian notation
     * @param destination_buffer, pointer to the buffer where data should be written
     * @param index, offset position where to start writing data in the destination_buffer
     * @param values, source buffer that should be written to the destination_buffer at specified position
     * @param n, number of items in source buffer
     * @return incremented index value by size of the written data. This is the offset position where next data can be written.
     */
    uint32_t pack_uint32(buffer_t*destination_buffer, uint32_t index, uint32_t *values, uint32_t n);
    /*!
     * @brief Unpacks uint32_t from given buffer with big endian notation taken into account and increments data pointer by size of the read bytes
     * @param source_buffer, double pointer (pointer to the pointer) to the source buffer from where data should be retrived.
     * @return decoded uint32_t value
     */
    uint32_t unpack_uint32(buffer_t**source_buffer);
    /*!
     * @brief Unpacks uint32_t array from given buffer with big endian notation taken into account and increments data pointer by size of the read bytes
     * @param source_buffer, double pointer (pointer to the pointer) to the source buffer from where data should be retrived.
     * @param destination_buffer, a pointer to the buffer where decoded data should be written
     * @param n, number of items to decode
     */
    void unpack_uint32(buffer_t**source_buffer,uint32_t*destination_buffer,uint32_t n);
    /*!
     * @brief Packs uint64_t value to the output buffer and preserves big endian notation
     * @param destination_buffer, pointer to the buffer where data should be written
     * @param index, offset position where to start writing data in the destination_buffer
     * @param value, uint64_t value that should be written to the buffer at specified position
     * @return incremented index value by size of the written data. This is the offset position where next data can be written.
     */
    uint32_t pack_uint64(buffer_t*destination_buffer,uint32_t index,uint64_t value);
    /*!
     * @brief Packs uint64_t array to the output buffer and preserves big endian notation
     * @param destination_buffer, pointer to the buffer where data should be written
     * @param index, offset position where to start writing data in the destination_buffer
     * @param values, source buffer that should be written to the destination_buffer at specified position
     * @param n, number of items in source buffer
     * @return incremented index value by size of the written data. This is the offset position where next data can be written.
     */
    uint32_t pack_uint64(buffer_t *destination_buffer, uint32_t index, uint64_t *value, uint64_t n);
    /*!
     * @brief Unpacks uint64_t from given buffer with big endian notation taken into account and increments data pointer by size of the read bytes
     * @param source_buffer, double pointer (pointer to the pointer) to the source buffer from where data should be retrived.
     * @return decoded uint64_t value
     */
    uint64_t unpack_uint64(buffer_t**source_buffer);
    /*!
     * @brief Unpacks uint64_t array from given buffer with big endian notation taken into account and increments data pointer by size of the read bytes
     * @param source_buffer, double pointer (pointer to the pointer) to the source buffer from where data should be retrived.
     * @param destination_buffer, a pointer to the buffer where decoded data should be written
     * @param n, number of items to decode
     */
    void unpack_uint64(buffer_t**source_buffer,uint64_t*destination_buffer,uint32_t n);
    /*!
     * @brief Packs frame head descriptor value to the output buffer and preserves big endian notation
     * @param destination_buffer, pointer to the buffer where data should be written
     * @param frame_head, reference to the frame head descriptor that should be packed to the destination_buffer
     * @return number of bytes written in the destination_buffer
     */
    uint32_t pack_frame_head_descriptor(buffer_t*destination_buffer, message::frame_head_descriptor_t& frame_head);
    /*!
     * @brief Packs datetime_t value to the output buffer and preserves big endian notation
     * @param destination_buffer, pointer to the buffer where data should be written
     * @param index, offset position where to start writing data in the destination_buffer
     * @param dtime, reference to the datetime that should be packed to the destination_buffer
     * @return incremented index value by size of the written data. This is the offset position where next data can be written.
     */
    uint32_t pack_datetime(buffer_t*destination_buffer,uint32_t index,message::datetime_t & dtime);
    /*!
     * @brief Unpacks datetime_t from given buffer with big endian notation taken into account and increments data pointer by size of the read bytes
     * @param source_buffer, double pointer (pointer to the pointer) to the source buffer from where data should be retrived.
     * @param dtime, a pointer to the datetime_t object where unpacked data should be written
     */
    void unpack_datetime(buffer_t**source_buffer,message::datetime_t*dtime);
    /*!
     * @brief Packs chirp_threat_descriptor_t to the output buffer and preserves big endian notation
     * @param destination_buffer, pointer to the buffer where data should be written
     * @param index, offset position where to start writing data in the destination_buffer
     * @param dchirp, reference to the chirp descriptor that should be packed to the destination_buffer
     * @return incremented index value by size of the written data. This is the offset position where next data can be written.
     */
    uint32_t pack_chirp_threat_descriptor(buffer_t*destination_buffer,uint32_t index,messageThreat::chirp_threat_descriptor_t & dchirp);
    /*!
     * @brief Unpacks chirp_threat_descriptor_t from given buffer with big endian notation taken into account and increments data pointer by size of the read bytes
     * @param source_buffer, double pointer (pointer to the pointer) to the source buffer from where data should be retrived.
     * @param dchirp, a pointer to the chirp_threat_descriptor_t object where unpacked data should be written
     */
    void unpack_chirp_threat_descriptor(buffer_t**source_buffer,messageThreat::chirp_threat_descriptor_t * dchirp);
    /*!
     * @brief Packs pulse_threat_descriptor_t to the output buffer and preserves big endian notation
     * @param destination_buffer, pointer to the buffer where data should be written
     * @param index, offset position where to start writing data in the destination_buffer
     * @param dpulse, reference to the puls descriptor that should be packed to the destination_buffer
     * @return incremented index value by size of the written data. This is the offset position where next data can be written.
     */
    uint32_t pack_pulse_threat_descriptor(buffer_t*destination_buffer,uint32_t index,messageThreat::pulse_threat_descriptor_t & dpulse);
    /*!
     * @brief Unpacks pulse_threat_descriptor_t from given buffer with big endian notation taken into account and increments data pointer by size of the read bytes
     * @param source_buffer, double pointer (pointer to the pointer) to the source buffer from where data should be retrived.
     * @param dpulse, a pointer to the pulse_threat_descriptor_t object where unpacked data should be written
     */
    void unpack_pulse_threat_descriptor(buffer_t**source_buffer,messageThreat::pulse_threat_descriptor_t * dpulse);
    /*!
     * @brief Packs threat_descriptor_t to the output buffer and preserves big endian notation
     * @param destination_buffer, pointer to the buffer where data should be written
     * @param index, offset position where to start writing data in the destination_buffer
     * @param dthreat, reference to the threat descriptor that should be packed to the destination_buffer
     * @return incremented index value by size of the written data. This is the offset position where next data can be written.
     */
    uint32_t pack_threat_descriptor(buffer_t*destination_buffer, uint32_t index, messageThreat::threat_descriptor_t &dthreat);
    /*!
     * @brief Unpacks threat_descriptor_t from given buffer with big endian notation taken into account and increments data pointer by size of the read bytes
     * @param source_buffer, double pointer (pointer to the pointer) to the source buffer from where data should be retrived.
     * @param dthreat, a pointer to the threat_descriptor_t object where unpacked data should be written
     */
    void unpack_threat_descriptor(buffer_t**source_buffer,messageThreat::threat_descriptor_t * dthreat);
    /*!
     * @brief Packs fft_descriptor_t to the output buffer and preserves big endian notation
     * @param destination_buffer, pointer to the buffer where data should be written
     * @param index, offset position where to start writing data in the destination_buffer
     * @param dfft, reference to the fft descriptor that should be packed to the destination_buffer
     * @return incremented index value by size of the written data. This is the offset position where next data can be written.
     */
    uint32_t pack_fft_descriptor(buffer_t*destination_buffer,uint32_t index, messageFFT::fft_descriptor_t &dfft);
    /*!
     * @brief Unpacks fft_descriptor_t from given buffer with big endian notation taken into account and increments data pointer by size of the read bytes
     * @param source_buffer, double pointer (pointer to the pointer) to the source buffer from where data should be retrived.
     * @param dfft, a pointer to the fft_descriptor_t object where unpacked data should be written
     */
    void unpack_fft_descriptor(buffer_t**source_buffer,messageFFT::fft_descriptor_t * dfft);
    /*!
     * @brief Packs frame head descriptor to the output buffer and preserves big endian notation
     * @warning Fields device unique id and version in frame head descriptor are filled based on device id and version saved in encoder
     * @param destination_buffer, pointer to the buffer where data should be written
     * @param msg_type, a frame identifier field
     * @return number of bytes written in the destination_buffer
     */
    uint32_t packHead(buffer_t*destination_buffer,message::frame_type_t msg_type);
    /*!
     * @brief Unpacks frame_head_descriptor_t from given buffer with big endian notation taken into account and increments data pointer by size of the read bytes
     * @param source_buffer, double pointer (pointer to the pointer) to the source buffer from where data should be retrived.
     * @param dhead, a pointer to the frame_head_descriptor_t object where unpacked data should be written
     */
    void unpackHead(buffer_t**source_buffer,message::frame_head_descriptor_t * dhead);
    /*!
     * @brief Counts number of bytes occupied in packed threat message
     * @param number_of_threats, number of the threats inside message
     * @return size of the packed threat message in bytes
     */
    uint32_t countBytesForThreatMessage();
    /*!
     * @brief Counts number of bytes occupied in packed FFT message
     * @param dfft, reference to the FFT descriptor for which packed message size should be calculated
     * @return size of the packed FFT message in bytes
     */
    uint32_t countBytesForFFTMessage(messageFFT::fft_descriptor_t&dfft);
    /*!
     * @brief Packs threat message based on the given threats descriptors and preserves big endian notation
     * @warning Fields device unique id and version are filled based on device id and version saved in encoder
     * @param bytes_written, pointer where to save number of bytes written to the buffer
     * @param dthreat, array of the threat descriptors which should be written in packed threat message
     * @param number_of_threats, number of items in dthreat array. Number of threat descriptors to pack
     * @return pointer to the dynamically allocated buffer with packed threat message. Note, this buffer should be deleted manually with explicit delete call!
     */
    buffer_t* packThreatMessage(uint32_t*bytes_written, messageThreat::threat_descriptor_t *dthreat);
    /*!
     * @brief Packs FFT message based on the given FFT descriptor and preserves big endian notation
     * @warning Fields device unique id and version are filled based on device id and version saved in encoder
     * @param bytes_written, pointer where to save number of bytes written to the buffer
     * @param dfft, FFT descriptor which should be written in packed FFT message
     * @return pointer to the dynamically allocated buffer with packed FFT message. Note, this buffer should be deleted manually with explicit delete call!
     */
    buffer_t* packFFTMessage(uint32_t*bytes_written, messageFFT::fft_descriptor_t &dfft);


    /*!
     * @brief Encoder constructor
     * @param dev_version, encoded device version that will be stored in each message packed by this encoder
     * @param device_unique_id, device unique id that will be stored in each message packed by this encoder
     */
    ProbeMessagesEncoder(uint32_t dev_version, uint32_t device_unique_id);
    /*!
     * @brief Packs message and preserves big endian notation
     * @warning Fields device unique id and version are filled based on device id and version saved in encoder
     * @param bytes_written, pointer where to save number of bytes written to the buffer
     * @param msg, pointer to the message that should be packed.
     * @return pointer to the dynamically allocated buffer with packed message. Note, this buffer should be deleted manually with explicit delete call!
     */
    buffer_t* packMessage(uint32_t*bytes_written, message *msg);
    /*!
     * @brief Unpacks message with big endian notation taken into account
     * @param buff, pointer to the buffer containing packed message
     * @param length, input buffer length
     * @return std::auto_ptr holding pointer to the decoded message that can be dynamically casted to the messageThreat or messageFFT
     */
    msg_auto_ptr unpackMessage(buffer_t*buff,uint32_t length);
    /*!
     * @brief Calculates Galileo time second since Galileo time epoch 21.08.1999 23:59:43
     * @return Galileo time in seconds since 21.08.1999 23:59:43
     */
    static double getGalileoSecondsSinceEpoch();
    /*!
     * @brief Calculates Galileo time second fraction
     * @return Galileo time second fraction
     */
    static double getGalileoSecondFraction();
    /*!
     * @brief Calculates Galileo 12-bits week number based on the operating system time
     * @return Galileo weeks number since 21.08.1999 23:59:43
     */
    static uint16_t getGalileoWeekNumber();
    /*!
     * @brief Calculates Galileo second of the week number based on the operating system time
     * @return Galileo second of the week number since 21.08.1999 23:59:43
     */
    static uint32_t getGalileoSecondOfTheWeekNumber();
    /*!
     * @brief Saves current operating system time into message::datetime_t
     * @param dest a pointer to the message::datetime_t object where current date and time should be saved
     */
    static void saveCurrentDateTime(message::datetime_t * dest);
};


#endif // PROBEMESSAGESENCODER_H
