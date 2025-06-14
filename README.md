# Fake omapi service written in cpp

Aim to run omapi service without java environment.  
To provide a method to decrypt in recovery like TWRP

keymint->omapi transport->omapi

omapi->SecureElementService->createTerminals->addTerminals->initialize->android.hardware.secure_element.ISecureElement/eSE1->init->ese_init->gpqese_open->TEEC_InitializeContext(✓),TEEC_OpenSession(✓) ~~<- Died here~~

~~OpenSession uses uuidEseGP(32552B22-89FE-42B4-8A45-A0C4E2DB0326) 
(a uuid of ese firmware in /vendor/firmware_mnt/image) 
which is related to ssgtzd 
Haven't tested further~~

Now go on
