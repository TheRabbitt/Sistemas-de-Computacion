/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
/* <DESC>
 * Very simple HTTP GET
 * </DESC>
 */
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

#define BTC 0
#define ETH 1

char btc_price[50];
char eth_price[50];
int current_currency;
double valorArsDolar=213; //Cotizacion actual del dolar en pesos argentinos (cambiar si hace falta)
double valorEurDolar=0.9; //Cotizacion actual del euro respecto al dolar (cambiar si hace falta)

extern double calc_conversion(double a,double b); //Funcion externa en archivo .asm
double obtenerCotizacion (double moneda,char monedaDigital[50]);

/* La función "write_data" se utiliza como una función de retorno de llamada para procesar los 
   datos recibidos de la API en la solicitud HTTP GET. Si la moneda actual es BTC, se copian los 
   datos recibidos en "btc_price"; si es ETH, se copian los datos recibidos en "eth_price".
*/
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t bytes = size * nmemb;
    if (current_currency == BTC) {
        memcpy(btc_price, ptr, bytes);
    } else if (current_currency == ETH) {
        memcpy(eth_price, ptr, bytes);
    }
    return bytes;
}

int main(void) {

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {

        // Solicitar precio de BTC
        current_currency = BTC;
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.binance.com/api/v3/ticker/price?symbol=BTCUSDT");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }

        // Solicitar precio de ETH
        current_currency = ETH;
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.binance.com/api/v3/ticker/price?symbol=ETHUSDT");
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }

        // Imprimir precios
        //printf("BTC price: %s\n", btc_price);                       
        //printf("ETH price: %s\n", eth_price);

        //Imprimir cotizaciones de ambas monedas digitales en ARS,USD Y EUR.
        
        printf("BTCenARS: %lf\n", obtenerCotizacion (valorArsDolar,btc_price));
        printf("BTCenUSD: %lf\n", obtenerCotizacion (1,btc_price));
        printf("BTCenEUR: %lf\n", obtenerCotizacion (valorEurDolar,btc_price));

        printf("ETHenARS: %lf\n", obtenerCotizacion (valorArsDolar,eth_price));
        printf("ETHenUSD: %lf\n", obtenerCotizacion (1,eth_price));
        printf("ETHenEUR: %lf\n", obtenerCotizacion (valorEurDolar,eth_price));
       

        // Liberar recursos
        curl_easy_cleanup(curl);
    }
    

    return 0;
}

double obtenerCotizacion (double moneda,char monedaDigital[50]) {

    char* punteroChar = strchr(monedaDigital, 'e');
        if (punteroChar != NULL) {
        punteroChar += 4;
        }
        else
        exit(EXIT_FAILURE);
        double us_value = strtod(punteroChar, NULL);
        double value = calc_conversion(us_value, moneda);
        return value;

}






