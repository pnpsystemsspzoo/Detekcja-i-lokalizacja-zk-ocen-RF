#include "SPIcommunicator.h"


SPIcommunicator :: SPIcommunicator()
{

}


SPIcommunicator :: ~SPIcommunicator()
{

}


int SPIcommunicator :: Init(const char *spi_dev_name)
{
    int result = 0;
    int ret = 0;
 

    spi_fd = open(spi_dev_name, O_RDWR);
	if(spi_fd <= 0)
    {
        printf("\nCannot open %s device!", spi_dev_name);
    }


    /*
	 * spi mode
	 */
	ret = ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		printf("\nCan't set spi mode");

	ret = ioctl(spi_fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		printf("\nCan't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		printf("\nCan't set bits per word");

	ret = ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		printf("\nCan't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		printf("\nCan't set max speed hz");

	ret = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		printf("\nCan't get max speed hz");

    result = ret;
    return result;
}


int SPIcommunicator :: TransferData(unsigned char* tx_buffer, unsigned char* rx_buffer, unsigned char bytes_to_transfer, unsigned char bytes_to_receive)
{
    int result = 0;
    int ret;
    struct spi_ioc_transfer tr;
    
    memset(&tr, 0, sizeof(tr));
	tr.tx_buf = (unsigned long)tx_buffer;
	tr.rx_buf = (unsigned long)rx_buffer;
	tr.len = bytes_to_transfer;
	tr.delay_usecs = delay;
	tr.speed_hz = 0;
	tr.bits_per_word = 0;
	
    ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret == 1)
		printf("\nCan't send spi message");

	// for (ret = 0; ret < bytes_to_receive; ret++) 
    // {
	// 	if (!(ret % 6))
	// 		puts("");
	// 	printf("%.2X ", rx_buffer[ret]);
	// }

    return result;
}

