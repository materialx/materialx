//
// TM & (c) 2019 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#include <MaterialXRuntime/RtNodeDef.h>
#include <MaterialXRuntime/RtObject.h>

#include <MaterialXRuntime/Private/PvtNodeDef.h>
#include <MaterialXRuntime/Private/PvtStage.h>

namespace MaterialX
{

RtNodeDef::RtNodeDef(const RtObject& obj) :
    RtPrim(obj)
{
}

const RtToken& RtNodeDef::typeName()
{
    return PvtNodeDef::typeName();
}

RtApiType RtNodeDef::getApiType() const
{
    return RtApiType::NODEDEF;
}

void RtNodeDef::setNodeTypeName(const RtToken& nodeTypeName)
{
    return hnd()->asA<PvtNodeDef>()->setNodeTypeName(nodeTypeName);
}

const RtToken& RtNodeDef::getNodeTypeName() const
{
    return hnd()->asA<PvtNodeDef>()->getNodeTypeName();
}

RtObject RtNodeDef::createAttribute(const RtToken& name, const RtToken& type, uint32_t flags)
{
    return hnd()->asA<PvtNodeDef>()->createAttribute(name, type, flags)->obj();
}

void RtNodeDef::removeAttribute(const RtToken& name)
{
    return hnd()->asA<PvtNodeDef>()->removeAttribute(name);
}

}