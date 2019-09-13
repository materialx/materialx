// TM & (c) 2019 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#ifndef MATERIALXRENDER_TEXTUREBAKER
#define MATERIALXRENDER_TEXTUREBAKER

/// @file
/// TextureBaker

#include <MaterialXCore/Document.h>
#include <MaterialXCore/Material.h>
#include <MaterialXRenderGlsl/GlslValidator.h>
#include <MaterialXGenGlsl/GlslShaderGenerator.h>
#include <MaterialXRenderGlsl/GLTextureHandler.h>
#include <MaterialXGenShader/Shader.h>

#include <MaterialXTest/RenderUtil.h>

#include <iostream>
#include <fstream>
#include <iostream>


namespace mx = MaterialX;

namespace MaterialX {

class TextureBaker;

/// A shared pointer to a TextureBaker
using TextureBakerPtr = shared_ptr<TextureBaker>;
/// A shared pointer to a const Document
using ConstTextureBakerPtr = shared_ptr<const TextureBaker>;

/// @class @GlslValidator
/// Helper class to perform validation of GLSL source code generated by the GLSL code generator.
///
/// There are two main interfaces which can be used. One which takes in a HwShader and one which
/// allows for explicit setting of shader stage code.
///
/// The main services provided are:
///     - Validation: All shader stages are compiled and atteched to a GLSL shader program.
///     - Introspection: The compiled shader program is examined for uniforms and attributes.
///     - Binding: Uniforms and attributes which match the predefined variables generated the GLSL code generator
///       will have values assigned to this. This includes matrices, attribute streams, and textures.
///     - Rendering: The program with bound inputs will be used to drawing geometry to an offscreen buffer.
///     An interface is provided to save this offscreen buffer to disk using an externally defined image handler.
///

class TextureBaker 
{
    public:
        /// TextureBaker constructor
        TextureBaker();

        /// Create a TextureBaker instance
        static TextureBakerPtr createTextureBaker(const std::string destinationShadingModel)
        {
            return std::make_shared<TextureBaker>();
        }

        /// Destructor
        ~TextureBaker() { }

        /// Saves freshly made images for various outputs to disk
        void bakeAllInputTextures(unsigned int frameBufferDim, const std::string fileSuffix, const FileSearchPath& searchPath, mx::ElementPtr elem, mx::GenContext& context, const std::string udim);
        /// Saves freshly made image for specific output to disk
        void bakeTextureFromElementInput(mx::ElementPtr elem, mx::GenContext& context);
        /// Saves materialX file for baked object
        void saveMtlx(mx::DocumentPtr& origDoc, mx::TypedElementPtr elem);
    protected:
        /// Getters and setters for variables
        void setFileSuffix(const std::string fileSuffix) { _fileSuffix = fileSuffix; }
        const std::string getFileSuffix() { return _fileSuffix; }
        void setFrameBufferDim(int frameBufferDim) { _frameBufferDim = frameBufferDim; }
        int getFrameBufferDim() { return _frameBufferDim; }
        void setSearchPath(const FileSearchPath searchPath) { _searchPath = searchPath; }
        FileSearchPath getSearchPath() { return _searchPath; }

        bool alreadyBaked(const std::string output) { return _bakedOutputs.count(output) == 0; }
        void recordBakedTexture(const std::string input, const std::string outputFile) { _bakedTextures[input] = outputFile; }
        void recordNodegraphInput(const std::string input, const std::string type) { _bakedOutputs[input] = type; }

        /// @name Texture Baking Housekeeping
        /// @{

        /// Internal context initialization for texture baking
        void prepareTextureSpace(mx::GenOptions& options, mx::ElementPtr input, const std::string udim);
        /// Internal context cleanup for texture baking
        void cleanup(mx::GenOptions& options);

        /// Our rasterizer that will do the rendering
        GlslValidatorPtr _rasterizer;
        /// Our shader generator
        ShaderGeneratorPtr _generator;

        /// Default file format for baked texture
        std::string _fileSuffix = ".png";

        /// dimensions for the texture
        unsigned int _frameBufferDim = 512;

        /// Path to look for textures
        FileSearchPath _searchPath;

        /// Map to keep track of textures baked so far
        std::map<string, string> _bakedTextures;
        /// Map to keep track of shader graph outputs baked so far
        std::map<string, string> _bakedOutputs;
};

}
#endif // MATERIALXRENDER_TEXTUREBAKER