#include <lvgl.h>
#include <SPI.h>
#include <WiFi.h>
#include "display.h"

char ssid[] = "Roe";
char pass[] = "123123123";
Display screen;


// Déclaration des boutons 
lv_obj_t *btn_ip;
lv_obj_t *btn_mac;
lv_obj_t *btn_system_time;
lv_obj_t *btn_memory_info;
lv_obj_t *btn_device_info;
lv_obj_t *btn_bus_info;

lv_obj_t *info_label;



void btn_ip_event_cb(lv_event_t *e) {
  Serial.println("Bouton Adresse ip cliqué !");
  String ip = "IP Address: " + WiFi.localIP().toString();
  lv_label_set_text(info_label, ip.c_str());
}

void btn_mac_event_cb(lv_event_t *e) {
  Serial.println("Bouton Adresse mac cliqué !");
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char macStr[40] = {0};
  sprintf(macStr, "MAC Address: %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  lv_label_set_text(info_label, macStr);
}

void btn_system_time_event_cb(lv_event_t *e) {
  Serial.println("Bouton Heure système cliqué !");
  
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  // Formater le temps pour l'affichage
  char timeStr[50];
  strftime(timeStr, sizeof(timeStr), "Date: %Y-%m-%d\nTime: %Hh %Mm %Ss", &timeinfo);

  // Mettre à jour l'étiquette d'informations avec le temps système
  lv_label_set_text(info_label, timeStr);
}

void btn_memory_info_event_cb(lv_event_t *e) {
  Serial.println("Bouton Informations mémoire cliqué !");
  size_t freeHeapKB = ESP.getFreeHeap() / 1024; // Convertir la mémoire RAM disponible en kilo-octets (KB)
  size_t freeFlashKB = ESP.getFreeSketchSpace() / 1024; // Convertir la mémoire flash disponible en kilo-octets (KB)

  char memInfoStr[50]; // Buffer pour stocker la chaîne de caractères
  snprintf(memInfoStr, sizeof(memInfoStr), "Free RAM: %u KB \nFree Flash memory: %u KB", freeHeapKB, freeFlashKB); // Formater la chaîne de caractères
  lv_label_set_text(info_label, memInfoStr); // Afficher la quantité de mémoire RAM et de mémoire flash disponibles
}

void btn_device_info_event_cb(lv_event_t *e) {
   Serial.println("Bouton Informations sur le périphérique cliqué !");
    
  
  // Obtention du modèle de puce ESP32
  String chipModel = ESP.getChipModel();

  // Création de la chaîne d'informations sur le périphérique
  String deviceInfo = "Model: " + chipModel + "\nMC: ESP32-S3-WROOM-1";

  // Affichage des informations sur l'écran
  lv_label_set_text(info_label, deviceInfo.c_str());
}

void btn_bus_info_event_cb(lv_event_t *e) {
   Serial.println("Bouton Informations sur le bus cliqué !");

  // Chaîne pour stocker les informations sur les interfaces disponibles
  String busInfo = "";

  // Vérification de la disponibilité de l'interface SPI
  #if defined(ESP32)
  busInfo += "ESP32: SPI available\n";
  #else
  busInfo += "SPI unavailable\n";
  #endif

  // Vérification de la disponibilité de l'interface I2C (Wire)
  #if defined(Wire)
  busInfo += "I2C (Wire) available\n";
  #else
  busInfo += "I2C (Wire) unavailable\n";
  #endif

  // Vérification de la disponibilité de l'interface Ethernet
  #if defined(Ethernet)
  busInfo += "Ethernet available\n";
  #else
  busInfo += "Ethernet unavailable\n";
  #endif

  // Vérification de la disponibilité de l'interface Bluetooth Serial (SerialBT)
  #if defined(SerialBT)
  busInfo += "SerialBT available\n";
  #else
  busInfo += "SerialBT unavailable\n";
  #endif


if (WiFi.status() == WL_CONNECTED) {
    busInfo += "WiFi available\n";
  } else {
    busInfo += "WiFi unavailable\n";
  }

  // Affichage des informations sur l'écran
  lv_label_set_text(info_label, busInfo.c_str());
}

void setup() {
  Serial.begin(9600);
  Serial.println("DEBUT FONCTION SETUP");

  // Connexion WiFi
  WiFi.begin(ssid, pass);
  WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connexion WiFi etablie !");

  configTime(3600, 0, "pool.ntp.org");


  // Initialisation de l'écran
  screen.init();
  Serial.println("Écran initialisé");

  // Création des boutons
  btn_ip = lv_btn_create(lv_scr_act());
  lv_obj_align(btn_ip, LV_ALIGN_TOP_LEFT, 10, 10);
  lv_obj_add_event_cb(btn_ip, btn_ip_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *label_ip = lv_label_create(btn_ip);
  lv_label_set_text(label_ip, "@ IP");

  btn_mac = lv_btn_create(lv_scr_act());
  lv_obj_align(btn_mac, LV_ALIGN_TOP_LEFT, 10, 60);
  lv_obj_add_event_cb(btn_mac, btn_mac_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *label_mac = lv_label_create(btn_mac);
  lv_label_set_text(label_mac, "@ MAC");

  btn_system_time = lv_btn_create(lv_scr_act());
  lv_obj_align(btn_system_time, LV_ALIGN_TOP_LEFT, 10, 110);
  lv_obj_add_event_cb(btn_system_time, btn_system_time_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *label_system_time = lv_label_create(btn_system_time);
  lv_label_set_text(label_system_time, "System time");

  btn_memory_info = lv_btn_create(lv_scr_act());
  lv_obj_align(btn_memory_info, LV_ALIGN_TOP_RIGHT, -10, 10);
  lv_obj_add_event_cb(btn_memory_info, btn_memory_info_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *label_memory_info = lv_label_create(btn_memory_info);
  lv_label_set_text(label_memory_info, "Memory info");

  btn_device_info = lv_btn_create(lv_scr_act());
  lv_obj_align(btn_device_info, LV_ALIGN_TOP_RIGHT, -10, 60);
  lv_obj_add_event_cb(btn_device_info, btn_device_info_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *label_device_info = lv_label_create(btn_device_info);
  lv_label_set_text(label_device_info, "Device info");

  btn_bus_info = lv_btn_create(lv_scr_act());
  lv_obj_align(btn_bus_info, LV_ALIGN_TOP_RIGHT, -10, 110);
  lv_obj_add_event_cb(btn_bus_info, btn_bus_info_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *label_bus_info = lv_label_create(btn_bus_info);
  lv_label_set_text(label_bus_info, "Commu");

  // Création de la zone d'affichage des informations
  info_label = lv_label_create(lv_scr_act());
  lv_label_set_text(info_label, "Informations");
  lv_obj_align(info_label, LV_ALIGN_BOTTOM_MID, 0, -20);
}

void loop() {
  screen.routine();
  delay(5);

}
