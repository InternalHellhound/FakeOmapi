# Fake omapi service written in cpp

Aim to run omapi service without java environment.  
To provide a method to decrypt in recovery like TWRP

keymint->omapi transport->omapi

omapi->SecureElementService->createTerminals->addTerminals->initialize->android.hardware.secure_element.ISecureElement/eSE1->init->ese_init->gpqese_open->TEEC_InitializeContext(✓),TEEC_OpenSession(x)  <- Died here
