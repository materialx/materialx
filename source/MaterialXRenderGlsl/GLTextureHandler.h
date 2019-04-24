//
// TM & (c) 2017 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#ifndef MATERIALX_GLTEXTUREHANDLER_H
#define MATERIALX_GLTEXTUREHANDLER_H

/// @file
/// OpenGL texture handler

#include <MaterialXRender/ImageHandler.h>

namespace MaterialX
{
/// Shared pointer to an OpenGL texture handler
using GLTextureHandlerPtr = std::shared_ptr<class GLTextureHandler>;

/// @class GLTextureHandler
/// An OpenGL texture handler class
class GLTextureHandler : public ImageHandler
{
  public:
    using ParentClass = ImageHandler;

    /// Static instance create function
    static GLTextureHandlerPtr create(ImageLoaderPtr imageLoader)
    {
        return std::make_shared<GLTextureHandler>(imageLoader);
    }

    /// Default constructor
    GLTextureHandler(ImageLoaderPtr imageLoader);

    /// Default destructor
    virtual ~GLTextureHandler() {}


    /// Utility to create a solid color color image
    /// This method will create an OpenGL texture resource and return it's resource identifier
    /// as part of the image description returned.
    /// @param color Color to set
    /// @param imageDesc Description of image updated during load.
    /// @return if creation succeeded
    bool createColorImage(const Color4& color,
                          ImageDesc& imageDesc) override;

    /// Acquire an image from disk.
    /// The first image loader which supports the file name extension will be used.
    /// This method will create an OpenGL texture resource and return it's resource identifier
    /// as part of the image description returned.
    /// @param fileName Path to file to load image from.
    /// @param imageDesc Description of image updated during load.
    /// @param generateMipMaps Generate mip maps if supported.
    /// @param fallbackColor Color of fallback image to use if failed to load.  If null is specified then
    /// no fallback image will be acquired.
    /// @return if load succeeded in loading image or created fallback image.
    bool acquireImage(const FilePath& filePath, ImageDesc &imageDesc, bool generateMipMaps, const Color4* fallbackColor) override;

    /// Bind an image. This method will bind the texture to an active texture
    /// unit as defined by the corresponding image description. The method
    /// will fail if there are not enough available image units to bind to.
    /// @param identifier Identifier for image description to bind.
    /// @param samplingProperties Sampling properties for the image
    /// @return true if succeded to bind
    bool bindImage(const string &identifier, const ImageSamplingProperties& samplingProperties) override;

    /// Utility to map an address mode enumeration to an OpenGL address mode
    static int mapAddressModeToGL(int addressModeEnum);

    /// Utility to map a filter type enumeration to an OpenGL filter type
    static int mapFilterTypeToGL(int filterTypeEnum);

    /// Returns the bound texture location for a given resource
    int getBoundTextureLocation(unsigned int resourceId) override;

  protected:
    /// Delete an image
    /// @param imageDesc Image description indicate which image to delete.
    /// Any OpenGL texture resource and as well as any CPU side reosurce memory will be deleted.
    void deleteImage(MaterialX::ImageDesc& imageDesc) override;

    /// Return restrictions specific to this handler
    const ImageDescRestrictions* getRestrictions() const override
    {
        return &_restrictions;
    }

    /// Returns the first free texture location that can be bound to.
    int getNextAvailableTextureLocation();

    /// Maximum number of available image units
    int _maxImageUnits;

    /// Support restrictions
    ImageDescRestrictions _restrictions;

    std::vector<unsigned int> _boundTextureLocations;
};

} // namespace MaterialX
#endif
