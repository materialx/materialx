//
// TM & (c) 2017 Lucasfilm Entertainment Company Ltd. and Lucasfilm Ltd.
// All rights reserved.  See LICENSE.txt for license.
//

#ifndef MATERIALX_GENSHADERLIBRARY_H
#define MATERIALX_GENSHADERLIBRARY_H

/// @file
/// Library-wide includes and types.  This file should be the first include for
/// any public header in the MaterialXGenShader library.

#include <MaterialXCore/Library.h>

#include <MaterialXCore/Exception.h>

#include <MaterialXGenShader/Export.h>

namespace MaterialX
{

class Shader;
class ShaderStage;
class ShaderGenerator;
class ShaderNode;
class ShaderGraph;
class ShaderInput;
class ShaderOutput;
class ShaderNodeImpl;
class GenOptions;
class GenContext;
class TypeDesc;

/// A string stream
using StringStream = std::stringstream;

/// Shared pointer to a Shader
using ShaderPtr = shared_ptr<Shader>;
/// Shared pointer to a ShaderStage
using ShaderStagePtr = shared_ptr<ShaderStage>;
/// Shared pointer to a ShaderGenerator
using ShaderGeneratorPtr = shared_ptr<ShaderGenerator>;
/// Shared pointer to a ShaderNodeImpl
using ShaderNodeImplPtr = shared_ptr<ShaderNodeImpl>;
/// Shared pointer to a GenContext
using GenContextPtr = shared_ptr<GenContext>;

template<class T> using CreatorFunction = shared_ptr<T>(*)();

/// @class ExceptionShaderGenError
/// An exception that is thrown when shader generation fails.
class MX_GENSHADER_API ExceptionShaderGenError : public Exception
{
  public:
    using Exception::Exception;
};

} // namespace MaterialX

#endif // MATERIALX_GENSHADERLIBRARY_H
