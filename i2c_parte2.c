
#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>


#define MODE_READ 0
#define MODE_WRITE 1
#define MAX_LEN 32
 
typedef enum {
    NO_ACTION,
    I2C_BEGIN,
    I2C_END
} i2c_init;
 
uint8_t  init = NO_ACTION;
uint16_t clk_div = BCM2835_I2C_CLOCK_DIVIDER_148;
uint8_t slave_address = 0x00;
uint32_t len = 0;
uint8_t  mode = MODE_READ;
 

int i;
uint8_t data;

int leerTemp()    // Imprime lectura de temperatura actual
{
    
    char buf[MAX_LEN];
    mode = MODE_READ;
    init=I2C_BEGIN;
    len=2;
    clk_div=2500;
    slave_address=77;
    char buffer[]={0x00}; // apunta al inicio de los registros
    if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return 1;
    }
    if (init == I2C_BEGIN)
    {
      if (!bcm2835_i2c_begin())
      {
        printf("bcm2835_i2c_begin failed. Are you running as root??\n");
        return 1;
      }
    }
    bcm2835_i2c_setSlaveAddress(slave_address);
    bcm2835_i2c_setClockDivider(clk_div);
    if (mode == MODE_READ) {
        bcm2835_i2c_write(buffer, 1); // poner puntero de la direccion en la posicion cero 
        bcm2835_i2c_read(buf, 1);
    }
   
    // This I2C end is done after a transfer if specified
    if (init == I2C_END) bcm2835_i2c_end();   
    bcm2835_close();
    return buf[0];
}

int setDataClock()
{
    mode = MODE_WRITE;
    init=I2C_BEGIN;
    clk_div=2500;
    slave_address=104;
    len= 8;
    char wbuf1[8]={0,0x00,0x00,0x12,0x01,0x01,0x01,0x01};
    if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return 1;
    }
    if (init == I2C_BEGIN)
    {
      if (!bcm2835_i2c_begin())
      {
        printf("bcm2835_i2c_begin failed. Are you running as root??\n");
        return 1;
      }
    }
    bcm2835_i2c_setSlaveAddress(slave_address);
    bcm2835_i2c_setClockDivider(clk_div);
    if (mode == MODE_WRITE) {
        data = bcm2835_i2c_write(wbuf1, len);
        
    }   
   
    // This I2C end is done after a transfer if specified
    if (init == I2C_END) bcm2835_i2c_end();   
    bcm2835_close();
}

int leerFecha(FILE *manejador)
{
    
    char buf[MAX_LEN];
    mode = MODE_READ;
    init=I2C_BEGIN;
    len=8;
    clk_div=2500;
    slave_address=104;
    char buffer[]={0x00};
    
    if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return 1;
    }
    if (init == I2C_BEGIN)
    {
      if (!bcm2835_i2c_begin())
      {
        printf("bcm2835_i2c_begin failed. Are you running as root??\n");
        return 1;
      }
    }
    bcm2835_i2c_setSlaveAddress(slave_address);
    bcm2835_i2c_setClockDivider(clk_div);
    if (mode == MODE_READ) {
        bcm2835_i2c_write(buffer, 1);
        bcm2835_i2c_read(buf, len); 
        
        if(buf[6]<10) fprintf(manejador,"0");  //Escritura en archivo
            fprintf(manejador,"%x/", buf[6]);
        if(buf[6]<10) printf("0");
            printf("%x/", buf[6]);
            
        if(buf[5]<10) fprintf(manejador,"0");   //Escritura en archivo
            fprintf(manejador,"%x/", buf[5]);    
        if(buf[5]<10) printf("0");
            printf("%x/",buf[5]);
            
        if(buf[4]<10) fprintf(manejador,"0");    //Escritura en archivo
            fprintf(manejador,"%x/", buf[4]);       
        if(buf[4]<10) printf("0");
            printf("%x", buf[4]);
            
            
        switch(buf[3]){
            case 1:
                fprintf(manejador," Mon ");  
                printf(" Mon ");
                break;
            case 2:
                fprintf(manejador," Tue "); 
                printf(" Tue ");
                break;
            case 3:
                fprintf(manejador," Wed "); 
                printf(" Wed ");
                break;
            case 4:
                fprintf(manejador," Thur "); 
                printf(" Thur ");
                break;
            case 5:
                fprintf(manejador," Fri ");
                printf(" Fri ");
                break;
            case 6:
                fprintf(manejador," Sat ");
                printf(" Sat ");
                break;
            case 7:
                fprintf(manejador," Sun ");
                printf(" Sun ");
                break;
            }
        
        if(buf[2]<10) fprintf(manejador,"0");    //Escritura en archivo
            fprintf(manejador,"%x:", buf[2]);  
        if(buf[2]<10) printf("0");
            printf("%x:", buf[2]);
            
        if(buf[1]<10) fprintf(manejador,"0");    //Escritura en archivo
            fprintf(manejador,"%x:", buf[1]);
        if(buf[1]<10) printf("0");
            printf("%x:",buf[1]);
        
        if(buf[0]<10) fprintf(manejador,"0");    //Escritura en archivo
            fprintf(manejador,"%x AM", buf[0]);
        if(buf[0]<10) printf("0");
            printf("%x AM", buf[0]);
            
                
    }
   
    // This I2C end is done after a transfer if specified
    if (init == I2C_END) bcm2835_i2c_end();   
    bcm2835_close();
}

int main(int argc, char **argv) {
    int tempA;
    int cont=1;
    setDataClock(); //inicializar reloj
    time_t base_time;
    base_time = time(NULL);
    long long bSeconds = base_time - 10;
    
    //Manejo de archivos
    FILE *manejador_archivo;
    
   manejador_archivo= fopen("datalog.txt","w");
    while(1) 
    {
        if(cont%3 == 0) 
        {
            rewind(manejador_archivo); //reiniciar fichero para llenar con los tres logs mas recientes
        }
        
        tempA=leerTemp();
        base_time= time(NULL);
        long long bNow = base_time;
        
        if((abs(bNow - bSeconds)>=10)) 
        {
            fprintf(manejador_archivo,"\nRECEIVER> ");
            printf("\nRECEIVER> ");
            fprintf(manejador_archivo,"Temperature: %ld ªC",tempA);
            printf("Temperature: %ld ªC",tempA);
            fprintf(manejador_archivo,"\nRECEIVER> ");
            printf("\nRECEIVER> ");
            fprintf(manejador_archivo,"Record %d: ",cont);
            printf("Record %d: ",cont);
            leerFecha(manejador_archivo);
            fprintf(manejador_archivo,"\n");
            printf("\n");
            cont++;           
        }
        bSeconds=bNow;
        if(tempA>=30)
        {
            fprintf(manejador_archivo,"\nRECEIVER> ");
            printf("RECEIVER> ");
            fprintf(manejador_archivo,"Temperature: %ld ªC",tempA);
            printf("Temperature: %ld ªC",tempA);
            fprintf(manejador_archivo,"\n");
            printf("\n");
            fprintf(manejador_archivo,"RECEIVER> ");
            printf("RECEIVER> ");
            fprintf(manejador_archivo,"Record %d: ",cont);
            printf("Record %d: ",cont);
            leerFecha(manejador_archivo);
            fprintf(manejador_archivo,"\n");
            printf("\n");
            cont++;
        }
        
        if(cont>=20) break;
        sleep(1);
    }
        fclose(manejador_archivo);

    return 0;
}
 
