#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

// Fornire le informazioni sul processo di generazione del token.
#include <addons/TokenHelper.h>
// Fornire le informazioni sulla stampa del payload del RTDB e altre funzioni di aiuto.
#include <addons/RTDBHelper.h>

// 1. Definisci le credenziali Wi-Fi
#define WIFI_SSID "TIM-39751438" // Sostituisci con il nome della tua rete Wi-Fi
#define WIFI_PASSWORD "EFuPktKzk6utU2y5a5SEkUUQ" // Sostituisci con la password della tua rete Wi-Fi

// 2. Definisci la chiave API e l'URL del database Firebase
#define API_KEY "AIzaSyCKuFLiX6_Hh-TZMEi2hnGHOZ5V2s9-GkA" // Sostituisci con la tua API Key
#define DATABASE_URL "https://esp32-celsius-default-rtdb.europe-west1.firebasedatabase.app" // Sostituisci con l'URL del tuo database (es. https://tuo-progetto-id.firebaseio.com/)

// Definisci gli oggetti di configurazione e autenticazione Firebase
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// Oggetto Firebase Data per le operazioni sul database (non usato per l'autenticazione ma utile in generale)
FirebaseData fbdo;

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Connessione al Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Configurazione Firebase
  firebaseConfig.api_key = API_KEY;
  firebaseConfig.database_url = DATABASE_URL;

  // Abilita la riconnessione automatica al Wi-Fi
  Firebase.reconnectWiFi(true);

  // Imposta il callback per la generazione del token (richiesto dalla libreria)
  // Questo callback si occupa di aggiornare automaticamente il token se scade.
  firebaseConfig.token_status_callback = tokenStatusCallback; // Dalla libreria TokenHelper.h

  Serial.println("------------------------------------");
  Serial.println("Tentativo di accesso anonimo a Firebase...");

  // Tentativo di accesso anonimo
  // Le stringhe vuote per email e password indicano un accesso anonimo [2, 4, 5, 7, 8]
  if (Firebase.signUp(&firebaseConfig, &firebaseAuth, "", "")) {
    Serial.println("Accesso anonimo riuscito!");
    Serial.print("User UID: ");
    Serial.println(firebaseAuth.token.uid.c_str()); // Stampa l'UID dell'utente anonimo [3, 5, 7, 14]
    
    // Ora puoi utilizzare l'oggetto fbdo per interagire con il Realtime Database
    // Ad esempio, per scrivere un dato:
    // if (Firebase.RTDB.setString(&fbdo, "/test/anonimo", "Connesso da ESP32 anonimo")) {
    //   Serial.println("Dato scritto con successo.");
    // } else {
    //   Serial.print("Errore scrittura dato: ");
    //   Serial.println(fbdo.errorReason());
    // }

  } else {
    Serial.print("Errore durante l'accesso anonimo: ");
    Serial.println(firebaseConfig.signer.signupError.message.c_str());
  }

  Serial.println("------------------------------------");
}

void loop() {
  // Il loop può essere vuoto per questa semplice prova di connessione
  // o puoi aggiungere logica per interagire con Firebase dopo l'autenticazione.
}
