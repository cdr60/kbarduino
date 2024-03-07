# KB Arduino

Boite à clés à base d'arduino :
- Matériels 
- 1 Arduino
- des MCP23017 en I2C (1 par bloc de 3 Clés) de 1 à 4 MCP. Adresses possibles : 0x21, 0x22, 0x23, 0x24
- connexion : via port série
- Ecran LCD 16x2 I2C à l'adresse 0x20
- Timeout : 15 secondes

# Protocole :
Demande de statut : 0x10 0x02 0x00
Réponse: 0x10 0x02 0xXX 0xYY 0x00 où XX et YY et fournissent les status en binaire des clés 

Demande de la libération de la clé N>0 et N<16 : 0x10 0x01 0xN
Réponse : le statut

Demande de lecture des paramères : 0x10 0xFF 0x00
Réponse : 0x10 0xFF 0xPP 0x08 0x07 0x02 0x3C, PP est la valeur de time out



  
