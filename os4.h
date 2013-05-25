/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

//#define SetRexxVarFromMsg IRexxSys->SetRexxVarFromMsg
#define SSL_CTX_new IAmiSSL->SSL_CTX_new
#define SSLv23_client_method IAmiSSL->SSLv23_client_method
#define SSL_CTX_set_default_verify_paths IAmiSSL->SSL_CTX_set_default_verify_paths
#define SSL_CTX_set_verify IAmiSSL->SSL_CTX_set_verify
#define SSL_new IAmiSSL->SSL_new
#define SSL_set_fd IAmiSSL->SSL_set_fd
#define SSL_clear IAmiSSL->SSL_clear
#define SSL_connect IAmiSSL->SSL_connect
#define SSL_get_peer_certificate IAmiSSL->SSL_get_peer_certificate
#define X509_NAME_oneline IAmiSSL->X509_NAME_oneline
#define X509_get_subject_name IAmiSSL->X509_get_subject_name
#define X509_get_issuer_name IAmiSSL->X509_get_issuer_name
#define X509_free IAmiSSL->X509_free
#define SSL_write IAmiSSL->SSL_write
#define SSL_read IAmiSSL->SSL_read
#define SSL_CTX_free IAmiSSL->SSL_CTX_free

#define AddSemaphore IExec->AddSemaphore
#define InitSemaphore IExec->InitSemaphore
#define NewList IExec->NewList

#define ObtainSemaphore IExec->ObtainSemaphore
#define ReleaseSemaphore IExec->ReleaseSemaphore

#define CodesetsSupportedA ICodesets->CodesetsSupportedA
#define CodesetsFreeA ICodesets->CodesetsFreeA
#define CodesetsFindA ICodesets->CodesetsFindA
#define CodesetsFindBestA ICodesets->CodesetsFindBestA
#define CodesetsFindBest ICodesets->CodesetsFindBest
#define CodesetsConvertStrA ICodesets->CodesetsConvertStrA
#define CodesetsConvertStr ICodesets->CodesetsConvertStr
#define CodesetsUTF8ToStr ICodesets->CodesetsUTF8ToStr
#define CodesetsUTF8Create ICodesets->CodesetsUTF8Create
#define CodesetsUTF8CreateA ICodesets->CodesetsUTF8CreateA
#define CodesetsUTF8ToStrA ICodesets->CodesetsUTF8ToStrA
#define CodesetsUTF8CreateA ICodesets->CodesetsUTF8CreateA
#define CodesetsIsValidUTF8 ICodesets->CodesetsIsValidUTF8
#define CodesetsIsLegalUTF8Sequence ICodesets->CodesetsIsLegalUTF8Sequence
#define CodesetsIsLegalUTF8 ICodesets->CodesetsIsLegalUTF8

#define DoMethod IIntuition->IDoMethod
#define GetAttr IIntuition->GetAttr
#define SetAttrs IIntuition->SetAttrs
#define DisposeObject IIntuition->DisposeObject
#define NewObject IIntuition->NewObject
#define NextObject IIntuition->NextObject
#define ObtainBitMapInstance IIntuition->ObtainBitMapInstance
#define ObtainBitMapSource IIntuition->ObtainBitMapSource
#define BitMapInstanceControl IIntuition->BitMapInstanceControl
#define ReleaseBitMapInstance IIntuition->ReleaseBitMapInstance
#define ReleaseBitMapSource IIntuition->ReleaseBitMapSource
#define LockPubScreen IIntuition->LockPubScreen
#define UnlockPubScreen IIntuition->UnlockPubScreen

#define SetAPen IGraphics->SetAPen
#define Move IGraphics->Move
#define Draw IGraphics->Draw
#define RectFill IGraphics->RectFill

#define RemTask IExec->RemTask

#define SuspendTask IExec->SuspendTask
#define CreateTask IExec->CreateTask
#define CreateTaskTags IExec->CreateTaskTags
#define AllocVec IExec->AllocVec
#define FreeVec IExec->FreeVec
#define FreeMem IExec->FreeMem
#define AllocMem IExec->AllocMem
#define AllocSignal IExec->AllocSignal
#define FreeSignal IExec->FreeSignal
#define OpenDevice IExec->OpenDevice
#define CloseDevice IExec->CloseDevice
#define SNPrintf IUtility->SNPrintf
#define Strnicmp IUtility->Strnicmp

#define OpenLibrary IExec->OpenLibrary
#define CloseLibrary IExec->CloseLibrary
#define NextTagItem IUtility->NextTagItem
#define DoSuperMethodA IIntuition->IDoSuperMethodA
#define Date2Amiga IUtility->Date2Amiga
#define Amiga2Date IUtility->Amiga2Date
#define GetInterface IExec->GetInterface
#define DropInterface IExec->DropInterface

#define Signal IExec->Signal
#define FindTask IExec->FindTask
#define AddPort IExec->AddPort
#define CreatePort IExec->CreatePort
#define WaitPort IExec->WaitPort
#define FindPort IExec->FindPort
#define ReplyMsg IExec->ReplyMsg
#define GetMsg IExec->GetMsg
#define PutMsg IExec->PutMsg
#define Forbid IExec->Forbid
#define Permit IExec->Permit
#define CreateMsgPort IExec->CreateMsgPort
#define DeletePort IExec->DeletePort
#define DeleteMsgPort IExec->DeleteMsgPort
#define RemPort IExec->RemPort
#define CreateIORequest IExec->CreateIORequest
#define DeleteIORequest IExec->DeleteIORequest
#define SystemTagList IDOS->SystemTagList
#define ChangeFilePosition IDOS->ChangeFilePosition
#define SystemTags IDOS->SystemTags

#define SetCurrentDirName IDOS->SetCurrentDirName
#define CurrentDir IDOS->CurrentDir
#define CreateDir IDOS->CreateDir
#define SetComment IDOS->SetComment
#define Rename IDOS->Rename
#define Lock IDOS->Lock
#define UnLock IDOS->UnLock
#define Examine IDOS->Examine
#define ExNext IDOS->ExNext
#define GetCurrentDir IDOS->GetCurrentDir
#define ExAll IDOS->ExAll
#define IoErr IDOS->IoErr
#define AllocDosObject IDOS->AllocDosObject
#define FreeDosObject IDOS->FreeDosObject
#define ExamineFH IDOS->ExamineFH
#define Delay IDOS->Delay
#define Seek IDOS->Seek
#define FSeek IDOS->FSeek
#define FOpen IDOS->FOpen
#define Open IDOS->Open
#define Close IDOS->Close
#define FClose IDOS->FClose
#define FGetC IDOS->FGetC
#define FGets IDOS->FGets
#define FPuts IDOS->FPuts
#define FPutC IDOS->FPutC
#define FFlush IDOS->FFlush
#define FWrite IDOS->FWrite
#define Read IDOS->Read
#define Write IDOS->Write
#define GetProgramDir IDOS->GetProgramDir
#define GetProgramName IDOS->GetProgramName

#define NameFromLock IDOS->NameFromLock
#define Wait IExec->Wait
#define Execute IDOS->Execute
#define DoIO IExec->DoIO
#define CheckIO IExec->CheckIO
#define SendIO IExec->SendIO
#define AbortIO IExec->AbortIO
#define WaitIO IExec->WaitIO

//#define strnicmp IUtility->Strnicmp
#define ToLower IUtility->ToLower
#define ToUpper IUtility->ToUpper

#define ObtainSocket ISocket->ObtainSocket
#define ReleaseSocket ISocket->ReleaseSocket
#define WaitSelect ISocket->WaitSelect
#define getsockname ISocket->getsockname
#define listen ISocket->listen
#define getpeername ISocket->getpeername
#define bind ISocket->bind
#define CloseSocket ISocket->CloseSocket
#define recv ISocket->recv
#define send ISocket->send
#define connect ISocket->connect
#define gethostbyname ISocket->gethostbyname
#define setsockopt ISocket->setsockopt
#define SocketBaseTags ISocket->SocketBaseTags
#define GetSocketEvents ISocket->GetSocketEvents
#define IoctlSocket ISocket->IoctlSocket
#define shutdown ISocket->shutdown
#define Errno ISocket->Errno
#define socket ISocket->socket
#define Inet_NtoA ISocket->Inet_NtoA
#define inet_addr ISocket->inet_addr
#define accept ISocket->accept
#define gethostid ISocket->gethostid
//#define inet_ntoa ISocket->inet_ntoa
#define inet_network ISocket->inet_network

#define OpenCatalog ILocale->OpenCatalog
#define CloseCatalog ILocale->CloseCatalog
#define GetCatalogStr ILocale->GetCatalogStr

#define GetDiskObject IIcon->GetDiskObject
#define FreeDiskObject IIcon->FreeDiskObject
#define GetFileSize IDOS->GetFileSize

#define DeleteFile IDOS->DeleteFile

#define FindToolType IIcon->FindToolType

//struct UtilityIFace *IUtility = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;

#define MUI_DisposeObject IMUIMaster->MUI_DisposeObject
#define MUI_CreateCustomClass IMUIMaster->MUI_CreateCustomClass
#define MUI_DeleteCustomClass IMUIMaster->MUI_DeleteCustomClass
#define MUI_ObtainPen IMUIMaster->MUI_ObtainPen
#define MUI_ReleasePen IMUIMaster->MUI_ReleasePen
#define MUI_Redraw IMUIMaster->MUI_Redraw

#define AslRequest IAsl->AslRequest
#define FreeAslRequest IAsl->FreeAslRequest

//comment above out to use -D__USE_INLINE__

#define NewDTObject IDataTypes->NewDTObject
#define DisposeDTObject IDataTypes->DisposeDTObject

#include "sys/filio.h"

#define MUI_NewObject IMUIMaster->MUI_NewObject
#define MUI_MakeObject IMUIMaster->MUI_MakeObject

#define AllocAslRequestTags IAsl->AllocAslRequestTags

#define SSL_get_current_cipher IAmiSSL->SSL_get_current_cipher
#define SSL_CIPHER_get_name IAmiSSL->SSL_CIPHER_get_name

