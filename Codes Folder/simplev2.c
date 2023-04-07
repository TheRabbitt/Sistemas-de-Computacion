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
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON-1.7.15/cJSON.h"

int main(void) {

//En este código, cJSON_Parse() se utiliza para analizar los datos de respuesta en formato JSON y 
//devolver un puntero al objeto cJSON resultante (json_root). Luego, los valores de cotización de 
//Bitcoin en las tres monedas deseadas se acceden mediante la función cJSON_GetObjectItem(),
//que recibe como argumentos el objeto cJSON y el nombre de clave correspondiente. 
//Por último, se imprimen las cotizaciones en la salida estándar y se libera la memoria utilizada  
//por el objetoc JSON con cJSON_Delete().

size_t handle_response(void *ptr, size_t size, size_t nmemb, void *userdata) {
  // Convierte los datos de respuesta en una cadena de caracteres
  char *response_str = (char *) ptr;

  // Analiza los datos de respuesta como JSON
  cJSON *json_root = cJSON_Parse(response_str);
  if (!json_root) {
    printf("Error al analizar la respuesta como JSON.\n");
    return 0;
  }

  // Accede a los valores de cotización de Bitcoin en las tres monedas deseadas
  double usd_quote = cJSON_GetObjectItem(json_root, "USD")->valuedouble;
  double eur_quote = cJSON_GetObjectItem(json_root, "EUR")->valuedouble;
  double ars_quote = cJSON_GetObjectItem(json_root, "ARS")->valuedouble;

  // Imprime las cotizaciones en las tres monedas en la salida estándar
  printf("Cotización de Bitcoin en USD: %lf\n", usd_quote);
  printf("Cotización de Bitcoin en EUR: %lf\n", eur_quote);
  printf("Cotización de Bitcoin en ARS: %lf\n", ars_quote);

  // Libera la memoria utilizada por el objeto cJSON
  cJSON_Delete(json_root);

  // Devuelve el tamaño de los datos recibidos
  return size * nmemb;
}



  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://min-api.cryptocompare.com/data/price?fsym=BTC&tsyms=USD,EUR,ARS");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_response);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "0d70a660-a30b-439e-8e00-abd435c6ca57");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    res = curl_easy_perform(curl);
  
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);
  }


  return 0;
}