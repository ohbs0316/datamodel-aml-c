/*******************************************************************************
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *******************************************************************************/

#include <string.h>
#include <string>

#include "Representation.h"
#include "AMLInterface.h"
#include "AMLException.h"

#include "camlrepresentation.h"
#include "camlinterface.h"
#include "camlerrorcodes.h"
#include "camlutils.h"

using namespace std;

static CAMLErrorCode ExceptionCodeToErrorCode(AMLResult result)
{
    switch (result)
    {
        case AML_INVALID_PARAM :
            return CAML_INVALID_PARAM;
        case AML_INVALID_FILE_PATH :
            return CAML_INVALID_FILE_PATH;
        case AML_INVALID_SCHEMA :
            return CAML_INVALID_AML_SCHEMA;
        case AML_INVALID_XML_STR :
            return CAML_INVALID_XML_STRING;
        case AML_NO_MEMORY :
            return CAML_NO_MEMORY;
        case AML_KEY_NOT_EXIST :
            return CAML_KEY_NOT_EXIST;
        case AML_KEY_ALREADY_EXIST :
            return CAML_KEY_ALREADY_EXIST;
        case AML_INVALID_DATA_TYPE :
            return CAML_INVALID_DATA_TYPE;

        case AML_INVALID_AMLDATA_NAME : //@TODO
        case AML_NOT_IMPL :
        case AML_KEY_VALUE_NOT_MATCH :
        case AML_ERROR :

        default : // AML_RESULT_OK
            return CAML_OK;
    }
}

CAMLErrorCode CreateRepresentation(const char* filePath, representation_t* repHandle)
{
    VERIFY_PARAM_NON_NULL(repHandle);
    VERIFY_PARAM_NON_NULL(filePath);

    *repHandle = new(std::nothrow) Representation(filePath);
    if (!repHandle)
    {
        return CAML_NO_MEMORY;
    }

    return CAML_OK;
}

CAMLErrorCode DestroyRepresentation(representation_t repHandle)
{
    VERIFY_PARAM_NON_NULL(repHandle);

    Representation* rep = static_cast<Representation*>(repHandle);
    delete rep;

    return CAML_OK;
}

CAMLErrorCode Representation_GetRepId(representation_t repHandle, char** repId)
{
    VERIFY_PARAM_NON_NULL(repHandle);
    VERIFY_PARAM_NON_NULL(repId);

    Representation* rep = static_cast<Representation*>(repHandle);
    string repIdStr = rep->getRepresentationId();

    try
    {
        *repId = ConvertStringToCharStr(repIdStr);
    }
    catch (const AMLException& e)
    {
        return CAML_NO_MEMORY;
    }

    return CAML_OK;
}

CAMLErrorCode Representation_GetConfigInfo(representation_t repHandle, amlObjectHandle_t* amlObjHandle)
{
    VERIFY_PARAM_NON_NULL(repHandle);
    VERIFY_PARAM_NON_NULL(amlObjHandle);

    try
    {
        //@TODO: should be this after Representaiton cpp API is modified.
        //const AMLObject& amlObj = static_cast<Representation*>(repHandle)->getConfigInfo();
        //*amlObjHandle = static_cast<amlObjectHandle_t>(const_cast<AMLObject*>(&amlObj));

        AMLObject* amlObj = static_cast<Representation*>(repHandle)->getConfigInfo();
        *amlObjHandle = static_cast<amlObjectHandle_t>(amlObj);
    }
    catch (const AMLException& e)
    {
        return CAML_INVALID_AML_SCHEMA;
    }

    return CAML_OK;
}

CAMLErrorCode Representation_DataToAml(const representation_t repHandle, const amlObjectHandle_t amlObjHandle, char** amlStr)
{
    VERIFY_PARAM_NON_NULL(repHandle);
    VERIFY_PARAM_NON_NULL(amlObjHandle);
    VERIFY_PARAM_NON_NULL(amlStr);

    Representation* rep = static_cast<Representation*>(repHandle);
    AMLObject* amlObj = static_cast<AMLObject*>(amlObjHandle);

    try
    {
        string amlString = rep->DataToAml(*amlObj);
        *amlStr = ConvertStringToCharStr(amlString);
    }
    catch (const AMLException& e)
    {
        return ExceptionCodeToErrorCode(e.code());
    }

    return CAML_OK;
}

CAMLErrorCode Representation_AmlToData(const representation_t repHandle, const char* amlStr, amlObjectHandle_t* amlObjHandle)
{
    VERIFY_PARAM_NON_NULL(repHandle);
    VERIFY_PARAM_NON_NULL(amlStr);
    VERIFY_PARAM_NON_NULL(amlObjHandle);

    Representation* rep = static_cast<Representation*>(repHandle);
    string amlString(amlStr, strlen(amlStr));

    try
    {
        AMLObject* amlObj = rep->AmlToData(amlString);
        *amlObjHandle = static_cast<amlObjectHandle_t>(amlObj);
    }
    catch (const AMLException& e)
    {
        return ExceptionCodeToErrorCode(e.code());
    }

    return CAML_OK;
}