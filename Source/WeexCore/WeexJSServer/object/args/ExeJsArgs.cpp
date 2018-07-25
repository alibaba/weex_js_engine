//
// Created by Darin on 29/05/2018.
//

#include "ExeJsArgs.h"
#include "../../utils/Utils.h"

ExeJsArgs::ExeJsArgs(std::vector<VALUE_WITH_TYPE *> param) {
    for (auto paramsObject : param) {
        auto pType = copyValueToSelf(paramsObject);
        if (pType != nullptr) {
            this->param.push_back(pType);
        }

    }

}

ExeJsArgs::ExeJsArgs(IPCArguments *arguments, size_t startCount) {
    size_t count = arguments->getCount();
    for (size_t i = startCount; i < count; ++i) {
        auto pType = getValueWithTypePtr();
        switch (arguments->getType(i)) {
            case IPCType::DOUBLE:
                pType->type = ParamsType::DOUBLE;
                pType->value.doubleValue = i;
                break;
            case IPCType::STRING: {
                pType->type = ParamsType::STRING;
                const IPCString *ipcstr = arguments->getString(i);
                pType->value.string = genWeexStringSS(ipcstr->content,ipcstr->length);

            }
                break;
            case IPCType::JSONSTRING: {

                pType->type = ParamsType::JSONSTRING;

                const IPCString *ipcstr = arguments->getString(i);

                pType->value.string = genWeexStringSS(ipcstr->content,ipcstr->length);
            }
                break;
            case IPCType::BYTEARRAY: {
                pType->type = ParamsType::BYTEARRAY;
                const IPCByteArray *array = arguments->getByteArray(i);
                pType->value.byteArray = genWeexByteArraySS(array->content,array->length);

            }
                break;
            default:
                pType->type = ParamsType::JSUNDEFINED;
                break;
        }

        this->param.push_back(pType);
    }

}

VALUE_WITH_TYPE *ExeJsArgs::getValueWithTypePtr() {
    auto *param = (VALUE_WITH_TYPE *) malloc(sizeof(VALUE_WITH_TYPE));
    if (param == nullptr)
        return nullptr;

    memset(param, 0, sizeof(VALUE_WITH_TYPE));

    return param;
}

VALUE_WITH_TYPE *ExeJsArgs::copyValueToSelf(VALUE_WITH_TYPE *value_with_type) {
    if (value_with_type == nullptr)
        return nullptr;

    auto pType = getValueWithTypePtr();
    pType->type = value_with_type->type;
    switch (value_with_type->type) {
        case ParamsType::DOUBLE: {
            pType->value.doubleValue = value_with_type->value.doubleValue;
        }
            break;
        case ParamsType::STRING:
        case ParamsType::JSONSTRING: {
            pType->value.string = genWeexStringSS(value_with_type->value.string->content,
                                                  value_with_type->value.string->length);

        }
            break;
        case ParamsType::BYTEARRAY: {
            pType->value.byteArray = genWeexByteArraySS(value_with_type->value.byteArray->content,
                                                        value_with_type->value.byteArray->length);
        }
            break;
        default:
            break;
    }
    return pType;
}
