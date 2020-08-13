//
// TM & (c) 2017 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#ifndef MATERIALX_SHADERTRANSLATOR_H
#define MATERIALX_SHADERTRANSLATOR_H

#include <MaterialXGenShader/Library.h>
#include <MaterialXGenShader/Shader.h>
#include <MaterialXGenShader/Util.h>

namespace MaterialX
{

/// A shared pointer to a TextureBaker
using ShaderTranslatorPtr = shared_ptr<class ShaderTranslator>;

/// @class ShaderTranslator
class ShaderTranslator
{
  public:
    /// Translates shaderRef to the destShader shading model
    void translateShader(ShaderRefPtr shaderRef, string destShader);

    /// Translates all the materials to the destShader shading model if translation exists.
    static void translateAllMaterials(DocumentPtr doc, string destShader);

    /// Returns set of all the available potential translations
    StringSet getAvailableTranslations(string start)
    {
        return _shadingTranslations[start];
    }

  protected:
    /// Constructor.
    static ShaderTranslatorPtr create(ConstDocumentPtr doc)
    {
        return ShaderTranslatorPtr(new ShaderTranslator(doc));
    }

    ShaderTranslator(ConstDocumentPtr doc);

    /// Reads shading translation nodes from the document
    void loadShadingTranslations();

    /// Connects translation node inputs from the original shaderRef
    void connectToTranslationInputs(ShaderRefPtr shaderRef);

    /// Connects translation node outputs to finalize shaderRef translation
    void connectTranslationOutputs(ShaderRefPtr shaderRef);

    /// Set that stores all the translation nodes in the document library
    StringSet _translationNodes;

    /// Map that stores all the potential destination shading models for given shading model
    std::unordered_map<string, StringSet> _shadingTranslations;

    /// Saved document that contains library for shading translation
    ConstDocumentPtr _doc;

    /// The inserted translation node
    NodePtr _translationNode;

    /// The nodegraph where translation node will be inserted
    NodeGraphPtr _ng;
};

} // namespace MaterialX

#endif
