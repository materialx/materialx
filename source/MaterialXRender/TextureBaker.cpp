//
// TM & (c) 2019 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//
#include <MaterialXRender/TextureBaker.h>
#include <MaterialXRenderGlsl/GlslValidator.h>
#include <MaterialXGenShader/Util.h>
#include <MaterialXTest/RenderUtil.h>

#include <MaterialXGenShader/HwShaderGenerator.h>
#include <MaterialXGenShader/DefaultColorManagementSystem.h>

#include <MaterialXRender/Util.h>
#include <MaterialXRender/LightHandler.h>

#include <MaterialXTest/GenShaderUtil.h>


namespace MaterialX
{
    TextureBaker::TextureBaker() 
    {}

    void TextureBaker::prepareBake(mx::GenContext& context, mx::ElementPtr input, const std::string udim) {
        std::string outputStr = input->getAttribute("nodegraph") + "_" + input->getAttribute("output");
        outputStr += (!udim.empty()) ? ("_" + udim) : "";
        context.setTextureInputString(input->getName());
        context.setNodeGraphOutputString(outputStr);
        context.setTextureInputType(input->getAttribute("type"));
    }

    void TextureBaker::cleanup(mx::GenContext& context)
    {
        context.setTextureBake(false);
        context.setTextureInputString("");
        context.setTextureInputType("");
        context.setNodeGraphOutputString("");
    }

    void TextureBaker::bakeAllInputTextures(unsigned int frameBufferDim, const std::string fileSuffix, const FileSearchPath& searchPath, mx::ElementPtr elem, mx::GenContext& context, const std::string udim)
    {
        context.setTextureBake(true);
        _fileSuffix = fileSuffix;
        _frameBufferDim = frameBufferDim;
        _searchPath = searchPath;
        for (ElementPtr input : elem->getChildren())
        {
            if (!input->getAttribute("nodegraph").empty() && !input->getAttribute("output").empty())
            {
                // If the output from the nodegraph hasn't been baked yet
                std::string outputStr = input->getAttribute("nodegraph") + "_" + input->getAttribute("output");
                if (!udim.empty())
                {
                    outputStr += "_" + udim;
                }
                if (_bakedOutputs.count(outputStr) == 0)
                {
                    prepareBake(context, input, udim);
                    bakeTextureFromElementInput(elem, context);
                    recordNodegraphInput(outputStr, input->getAttribute("type"));
                }
                recordBakedTexture(input->getName(), outputStr);
            }
        }
        cleanup(context);
    }


    void TextureBaker::bakeTextureFromElementInput(mx::ElementPtr elem, mx::GenContext& context)
    {
        _rasterizer = mx::GlslValidator::create(_frameBufferDim);
        mx::StbImageLoaderPtr stbLoader = mx::StbImageLoader::create();
        mx::GLTextureHandlerPtr imageHandler = mx::GLTextureHandler::create(stbLoader);
        imageHandler->setSearchPath(_searchPath);
        _rasterizer->setImageHandler(imageHandler);
        _rasterizer->initialize();

        _generator = mx::GlslShaderGenerator::create();

        const std::string name = "" + elem->getName() + "_" + context.getNodeGraphOutputString();
        ShaderPtr shader = _generator->generate("" + name + "_baker", elem, context);
        std::string vertexShader = shader->getSourceCode(mx::Stage::VERTEX);
        std::string pixelShader = shader->getSourceCode(mx::Stage::PIXEL);

        _rasterizer->validateCreation(shader);
        _rasterizer->renderToScreenSpaceQuad(context);
        std::string filename = elem->getDocument()->getSourceUri();
        filename.erase(filename.find_last_of("\\"));
        filename += ("\\") + name + _fileSuffix;
        _rasterizer->save(filename, false);
    }

    void TextureBaker::saveMtlx(mx::DocumentPtr& origDoc, mx::TypedElementPtr elem)
    {
        // create filename
        std::string filename = origDoc->getSourceUri();
        filename.insert(filename.find(".mtlx"), "_bake");
        // create doc
        mx::DocumentPtr bakedTextureDoc = mx::createDocument();
        // copy over all geominfo
        mx::GeomInfoPtr newGeom;
        for (mx::GeomInfoPtr geom : origDoc->getGeomInfos())
        {
            newGeom = bakedTextureDoc->addGeomInfo(geom->getName(), geom->getGeom());
            newGeom = geom;
            for (mx::GeomAttrPtr attr : geom->getGeomAttrs())
            {
                bakedTextureDoc->getGeomInfo(geom->getName())->setGeomAttrValue(attr->getName(), attr->getType(), attr->getValueString());
            }
        }

        // create nodegraph
        mx::NodeGraphPtr ng = bakedTextureDoc->addNodeGraph("NG_imgs");
        ng->setColorSpace("srgb_texture");
        for (std::map<std::string, std::string>::iterator it = _bakedOutputs.begin(); it != _bakedOutputs.end(); ++it)
        {
            // add the image node in the node graph
            mx::NodePtr img_node = ng->addNode("image", "" + it->first + "_image",it->second);
            mx::ParameterPtr param = img_node->addParameter("file", "filename");
            param->setValueString("" + elem->getName() + "_" + it->first + _fileSuffix);
            std::string outputStr = it->first;
            std::string nodeName = img_node->getName();

            if (it->first.find("normal") != std::string::npos)
            {
                mx::NodePtr normalmap_node = ng->addNode("normalmap", "" + it->first + "_normalmap", it->second);
                mx::InputPtr input = normalmap_node->addInput("in", it->second);
                input->setNodeName(nodeName);
                nodeName = normalmap_node->getName();
            }

            // add the output node in the node graph
            mx::OutputPtr output = ng->addOutput(outputStr, it->second);
            output->setNodeName(nodeName);
        }

        // create translated mat & shaderref
        mx::MaterialPtr baked_mat = bakedTextureDoc->addMaterial("baked_material");
        mx::ShaderRefPtr shaderRef = baked_mat->addShaderRef("" + elem->getName() + "_baked", elem->getAttribute("Shading model"));

        // fill in doc contents
        for (mx::ElementPtr input : elem->getChildren())
        {
            std::string name = input->getName();
            std::string type = input->getAttribute("type");
            mx::BindInputPtr bindInput = shaderRef->addBindInput(name, type);
            if (_bakedTextures.find(name) != _bakedTextures.end())
            {
                // add the bind input in shaderref
                bindInput->setOutputString(_bakedTextures[name]);
                bindInput->setNodeGraphString(ng->getName());
            }
            else
            {
                bindInput->setValueString(input->getAttribute("value"));
            }
        }

        mx::XmlWriteOptions writeOptions;
        writeOptions.writeXIncludeEnable = false;
        mx::writeToXmlFile(bakedTextureDoc, filename, &writeOptions);
        _bakedOutputs.clear();
        _bakedTextures.clear();
    }
}