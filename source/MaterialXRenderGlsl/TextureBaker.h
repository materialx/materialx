//
// TM & (c) 2019 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#ifndef MATERIALX_TEXTUREBAKER
#define MATERIALX_TEXTUREBAKER

/// @file
/// Texture baking functionality

#include <MaterialXCore/Unit.h>

#include <MaterialXRenderGlsl/GlslRenderer.h>
#include <MaterialXRenderGlsl/GLTextureHandler.h>

#include <MaterialXGenGlsl/GlslShaderGenerator.h>

namespace MaterialX
{

/// A shared pointer to a TextureBaker
using TextureBakerPtr = shared_ptr<class TextureBaker>;

/// Baked document list of shader node and it's corresponding baked Document
using ListofBakedDocuments = std::vector<std::pair<std::string, DocumentPtr>>;

/// @class TextureBaker
/// A helper class for baking procedural material content to textures.
/// TODO: Add support for graphs containing geometric nodes such as position
///       and normal.
class TextureBaker : public GlslRenderer
{
  public:
    static TextureBakerPtr create(unsigned int width = 1024, unsigned int height = 1024, Image::BaseType baseType = Image::BaseType::UINT8)
    {
        return TextureBakerPtr(new TextureBaker(width, height, baseType));
    }

    /// Set the file extension for baked textures.
    void setExtension(const string& extension)
    {
        _extension = extension;
    }

    /// Return the file extension for baked textures.
    const string& getExtension() const
    {
        return _extension;
    }

    /// Set the color space in which color textures are encoded.
    ///
    /// By default, this color space is srgb_texture, and color inputs are
    /// automatically transformed to this space by the baker.  If another color
    /// space is set, then the input graph is responsible for transforming
    /// colors to this space.
    void setColorSpace(const string& colorSpace)
    {
        _colorSpace = colorSpace;
    }

    /// Return the color space in which color textures are encoded.
    const string& getColorSpace() const
    {
        return _colorSpace;
    }

    /// Set the distance unit to which textures are baked.  Defaults to meters.
    void setDistanceUnit(const string& unitSpace)
    {
        _distanceUnit = unitSpace;
    }

    /// Return the distance unit to which textures are baked.
    const string& getDistanceUnit() const
    {
        return _distanceUnit;
    }

    /// Set whether images should be averaged to generate constants.  Defaults to false.
    void setAverageImages(bool enable)
    {
        _averageImages = enable;
    }

    /// Return whether images should be averaged to generate constants.
    bool getAverageImages()
    {
        return _averageImages;
    }

    /// Set whether uniform textures should be stored as constants.  Defaults to true.
    void setOptimizeConstants(bool enable)
    {
        _optimizeConstants = enable;
    }

    /// Return whether uniform textures should be stored as constants.
    bool getOptimizeConstants()
    {
        return _optimizeConstants;
    }

    /// Set the output location for baked texture images.  Defaults to the root folder
    /// of the destination material.
    void setOutputImagePath(const FilePath& outputImagePath)
    {
        _outputImagePath = outputImagePath;
    }

    /// Get the current output location for baked texture images.
    const FilePath& getOutputImagePath()
    {
        return _outputImagePath;
    }

    /// Set the "libraries" search path location. Otherwise will use getDefaultSearchPath()
    void setCodeSearchPath(const FileSearchPath& codesearchPath)
    {
        _codeSearchPath = codesearchPath;
    }

    /// Get report of baking results
    string getBakingReport() const
    {
         return (_bakingReport.str());
    }

    /// Clear report of baking results
    void clearBakingReport()
    {
        _bakingReport.clear();
    }

    /// Set the name of the baked graph element.
    void setBakedGraphName(const string& name)
    {
        _bakedGraphName= name;
    }

    /// Return the name of the baked graph element.
    const string& getBakedGraphName() const
    {
        return _bakedGraphName;
    }

    /// Set the name of the baked geometry info element.
    void setBakedGeomInfoName(const string& name)
    {
        _bakedGeomInfoName = name;
    }

    /// Return the name of the baked geometry info element.
    const string& getBakedGeomInfoName() const
    {
        return _bakedGeomInfoName;
    }

    /// Set up the unit definitions to be used in baking.
    void setupUnitSystem(DocumentPtr unitDefinitions);

    /// Bake textures for all graph inputs of the given shader.
    void bakeShaderInputs(NodePtr material, NodePtr shader, GenContext& context, const string& udim = EMPTY_STRING);

    /// Bake a texture for the given graph output.
    void bakeGraphOutput(OutputPtr output, GenContext& context, const FilePath& filename);

    /// Optimize baked textures before writing.
    void optimizeBakedTextures(NodePtr shader);

    /// Write the baked material with textures to a document.
    DocumentPtr bakeMaterial(NodePtr shader, const StringVec& udimSet);

    /// Utility which returns a list of baked documents for each material in the input document.
    ListofBakedDocuments createBakeDocuments(DocumentPtr doc, const FileSearchPath& imageSearchPath);

    /// Bake all materials in a document and write to disk one document per material. The provided filename is used to create a unique output filename for each baked material.
    FilePathVec bakeAllMaterials(DocumentPtr doc, const FileSearchPath& imageSearchPath, const FilePath& outputFileName);

  protected:
    TextureBaker(unsigned int width, unsigned int height, Image::BaseType baseType);

    // Generate a texture filename for the given graph output.
    FilePath generateTextureFilename(OutputPtr output, const string& srName, const string& udim);

  protected:
    class BakedImage
    {
      public:
        ImagePtr image;
        bool isUniform = false;
        Color4 uniformColor;
        FilePath filename;
    };
    class BakedConstant
    {
      public:
        Color4 color;
        bool isDefault = false;
    };
    using BakedImageVec = vector<BakedImage>;
    using BakedImageMap = std::unordered_map<OutputPtr, BakedImageVec>;
    using BakedConstantMap = std::unordered_map<OutputPtr, BakedConstant>;

    using WorldSpaceInputs = std::unordered_map<string, NodePtr>;

  protected:
    string _extension;
    string _colorSpace;
    string _distanceUnit;
    string _targetColorSpace;
    bool _averageImages;
    bool _optimizeConstants;
    FilePath _outputImagePath;
    string _bakedGraphName;
    string _bakedGeomInfoName;
    FileSearchPath _codeSearchPath;
    std::stringstream _bakingReport;

    ShaderGeneratorPtr _generator;
    ConstNodePtr _material;
    WorldSpaceInputs _worldSpaceShaderInputs;
    BakedImageMap _bakedImageMap;
    BakedConstantMap _bakedConstantMap;
};

} // namespace MaterialX

#endif
