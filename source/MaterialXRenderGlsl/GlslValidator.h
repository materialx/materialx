//
// TM & (c) 2017 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#ifndef MATERIALX_GLSLVALIDATOR_H
#define MATERIALX_GLSLVALIDATOR_H

/// @file
/// GLSL code validator

#include <MaterialXRender/ShaderValidator.h>
#include <MaterialXRender/ImageHandler.h>
#include <MaterialXRenderHw/SimpleWindow.h>
#include <MaterialXRenderGlsl/GLUtilityContext.h>
#include <MaterialXRenderGlsl/GlslProgram.h>
#include <MaterialXGenShader/GenContext.h>

namespace MaterialX
{

// Shared pointer to a GlslProgram
using GlslValidatorPtr = std::shared_ptr<class GlslValidator>;

/// @class GlslValidator
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
class GlslValidator : public ShaderValidator
{
  public:
    /// Create a GLSL validator instance
    static GlslValidatorPtr create(unsigned int res = 512);

    /// Destructor
    virtual ~GlslValidator();

    /// @name Setup
    /// @{

    /// Internal initialization of stages and OpenGL contstructs
    /// required for program validation and rendering.
    /// An exception is thrown on failure.
    /// The exception will contain a list of initialization errors.
    void initialize() override;

    /// @}
    /// @name Validation
    /// @{

    /// Validate creation of program based on an input shader
    /// @param shader Input HwShader
    void validateCreation(const ShaderPtr shader) override;

    /// Validate creation of program based on shader stage source code.
    /// @param stages Map of name and source code for the shader stages.
    void validateCreation(const StageMap& stages) override;

    /// Validate inputs for the program
    void validateInputs() override;

    /// Perform validation that inputs can be bound to and
    /// rendered with. Rendering is to an offscreen hardware buffer.
    void validateRender() override;

    /// @}
    /// @name Texture Baking
    /// @{

    /// Render output as a texture to screenspace quad to an 
    /// offscreen hardware buffer 
    /// @param context Context for the shader
    void renderScreenSpaceQuad(GenContext& context);

    /// @}
    /// @name Utilities
    /// @{

    /// Save the current contents the offscreen hardware buffer to disk.
    /// @param filePath Name of file to save rendered image to.
    /// @param floatingPoint Format of output image is floating point.
    void save(const FilePath& filePath, bool floatingPoint) override;

    /// Return the GLSL program wrapper class
    MaterialX::GlslProgramPtr program()
    {
        return _program;
    }

    /// @}

  protected:
    /// Constructor
    GlslValidator(unsigned int res = 512);

    /// Internal cleanup of stages and OpenGL constructs
    void cleanup();

    /// @name Target handling
    /// @{

    /// Create a offscreen target used for rendering.
    bool createTarget();
    /// Delete any created offscreen target.
    void deleteTarget();
    /// Bind or unbind any created offscree target.
    bool bindTarget(bool bind);

    /// @}
    /// @name Program bindings
    /// @{

    /// Update viewing information
    /// @param eye Eye position
    /// @param center Center of focus 
    /// @param up Up vector
    /// @param viewAngle Viewing angle in degrees
    /// @param nearDist Distance to near plane
    /// @param farDist Distance to far plane
    /// @param objectScale Scale to apply to geometry
    void updateViewInformation(const Vector3& eye,
                               const Vector3& center,
                               const Vector3& up,
                               float viewAngle,
                               float nearDist,
                               float farDist,
                               float objectScale);

  private:
    /// Utility to check for OpenGL context errors.
    /// Will throw an ExceptionShaderValidationError exception which will list of the errors found
    /// if any errors encountered.
    void checkErrors();

    /// GLSL program.
    GlslProgramPtr _program;

    /// Hardware color target (texture)
    unsigned int _colorTarget;

    /// Hardware depth target (texture)
    unsigned int _depthTarget;

    /// Hardware frame buffer object
    unsigned int _frameBuffer;

    /// Width of the frame buffer / targets to use.
    unsigned int _frameBufferWidth;
    /// Height of the frame buffer / targets to use.
    unsigned int _frameBufferHeight;

    /// Flag to indicate if validator has been initialized properly.
    bool _initialized;

    /// Data type being rendered out to texture
    std::string _type = "color3";

    /// Dummy window for OpenGL usage.
    SimpleWindowPtr _window;
    /// Dummy OpenGL context for OpenGL usage
    GLUtilityContextPtr _context;
};

} // namespace MaterialX

#endif
