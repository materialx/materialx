//
// TM & (c) 2017 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#include <MaterialXTest/Catch/catch.hpp>

#include <MaterialXCore/Document.h>
#include <MaterialXCore/Value.h>

namespace mx = MaterialX;

TEST_CASE("Material", "[material]")
{
    mx::DocumentPtr doc = mx::createDocument();

    // Create a base shader nodedef.
    mx::NodeDefPtr simpleSrf = doc->addNodeDef("ND_simpleSrf", "surfaceshader", "simpleSrf");
    simpleSrf->setInputValue("diffColor", mx::Color3(1.0f));
    mx::InputPtr specColor = simpleSrf->setInputValue("specColor", mx::Color3(0.0f));
    mx::InputPtr roughness = simpleSrf->setInputValue("roughness", 0.25f);
    mx::TokenPtr texId = simpleSrf->setTokenValue("texId", "01");
    REQUIRE(simpleSrf->getInputValue("diffColor")->asA<mx::Color3>() == mx::Color3(1.0f));
    REQUIRE(simpleSrf->getInputValue("specColor")->asA<mx::Color3>() == mx::Color3(0.0f));
    REQUIRE(simpleSrf->getInputValue("roughness")->asA<float>() == 0.25f);
    REQUIRE(simpleSrf->getTokenValue("texId") == "01");

    // Create an inherited shader nodedef.
    mx::NodeDefPtr anisoSrf = doc->addNodeDef("ND_anisoSrf", "surfaceshader", "anisoSrf");
    anisoSrf->setInheritsFrom(simpleSrf);
    anisoSrf->setInputValue("anisotropy", 0.0f);
    REQUIRE(anisoSrf->getInheritsFrom() == simpleSrf);
}
