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

#define BTC 0
#define ETH 1

char btc_price[50];
char eth_price[50];
int current_currency;

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
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }

        // Solicitar precio de ETH
        current_currency = ETH;
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.binance.com/api/v3/ticker/price?symbol=ETHUSDT");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }

        // Imprimir precios
        printf("BTC price: %s\n", btc_price);
        printf("ETH price: %s\n", eth_price);

        // Liberar recursos
        curl_easy_cleanup(curl);
    }

    return 0;
}




