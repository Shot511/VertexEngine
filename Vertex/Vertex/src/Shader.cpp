#include "Shader.h"
#include "CoreAssetManager.h"
#include "VertexEngineCore.h"

#include <glm\gtc\type_ptr.hpp>
#include <memory>

Shader::Shader(const std::string & vertexShaderFilename,
               const std::string & fragmentShaderFilename,
               const std::string & geometryShaderFilename, 
               const std::string & tessellationControlShaderFilename, 
               const std::string & tessellationEvaluationShaderFilename, 
               const std::string & computeShaderFilename) : program_id(0), isLinked(false)
{
    std::string shaderVersion = "#version " + std::to_string(MIN_GL_VERSION_MAJOR) + std::to_string(MIN_GL_VERSION_MINOR) + "0\n\n";

    const std::string shaderCodes[6] = { CoreAssetManager::loadFile(vertexShaderFilename), 
                                         CoreAssetManager::loadFile(fragmentShaderFilename), 
                                         CoreAssetManager::loadFile(geometryShaderFilename),
                                         CoreAssetManager::loadFile(tessellationControlShaderFilename),
                                         CoreAssetManager::loadFile(tessellationEvaluationShaderFilename),
                                         CoreAssetManager::loadFile(computeShaderFilename)
                                       };

    const std::string filenames[6] = { vertexShaderFilename, 
                                       fragmentShaderFilename, 
                                       geometryShaderFilename,
                                       tessellationControlShaderFilename,
                                       tessellationEvaluationShaderFilename,
                                       computeShaderFilename
                                     };

    /* Check if compute shader's source code is the only one available. */
    if (!shaderCodes[5].empty())
    {
        for (auto & shaderCode : shaderCodes)
        {
            if (!shaderCode.empty())
            {
                fprintf(stderr, "Compute shader can't be created due to presence of other shader source codes.\n");
                return;
            }
        }
    }

    program_id = glCreateProgram();

    if (program_id == 0)
    {
        fprintf(stderr, "Error while creating program object.\n");
        return;
    }

    for (int i = 0; i < sizeof(shaderCodes) / sizeof(std::string); ++i)
    {
        if (shaderCodes[i].empty())
        {
            continue;
        }

        GLuint shaderType = 0;
        
        if (i == 0)
            shaderType = GL_VERTEX_SHADER;
        else
        if (i == 1)
            shaderType = GL_FRAGMENT_SHADER;
        else
        if (i == 2)
            shaderType = GL_GEOMETRY_SHADER;
        else
        if (i == 3)
            shaderType = GL_TESS_CONTROL_SHADER;
        else
        if (i == 4)
            shaderType = GL_TESS_EVALUATION_SHADER;
        else
        if (i == 5)
            shaderType = GL_COMPUTE_SHADER;

        if (shaderType == 0)
        {
            fprintf(stderr, "Error! Wrong shader type.\n");
            continue;
        }

        GLuint shaderObject = glCreateShader(shaderType);

        if (shaderObject == 0)
        {
            fprintf(stderr, "Error while creating %s.\n", filenames[i].c_str());
            continue;
        }

        const char *shaderCode[2] = { shaderVersion.c_str(), shaderCodes[i].c_str() };

        glShaderSource (shaderObject, 2, shaderCode, NULL);
        glCompileShader(shaderObject);

        GLint result;
        glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

        if (result == GL_FALSE)
        {
            fprintf(stderr, "%s compilation failed!\n", filenames[i].c_str());

            GLint logLen;
            glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &logLen);

            if (logLen > 0)
            {
                char * log = (char *)malloc(logLen);

                GLsizei written;
                glGetShaderInfoLog(shaderObject, logLen, &written, log);

                fprintf(stderr, "Shader log: \n%s", log);
                free(log);
            }

            continue;
        }

        glAttachShader(program_id, shaderObject);
        glDeleteShader(shaderObject);
    }

    link();
    setupUnifomBuffers();

    //getchar();
}

Shader::~Shader()
{
    if (program_id != 0)
    {
        glDeleteProgram(program_id);
        program_id = 0;
    }

    for (UBO * ubo : uniformBlocks)
    {
        delete ubo;
        ubo = nullptr;
    }
}

bool Shader::link()
{
    glLinkProgram(program_id);

    GLint status;
    glGetProgramiv(program_id, GL_LINK_STATUS, &status);

    if (status == GL_FALSE)
    {
        fprintf(stderr, "Failed to link shader program!\n");

        GLint logLen;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &logLen);

        if (logLen > 0)
        {
            char* log = (char*)malloc(logLen);
            GLsizei written;
            glGetProgramInfoLog(program_id, logLen, &written, log);

            fprintf(stderr, "Program log: \n%s", log);
            free(log);
        }
    }
    else
    {
        isLinked = true;
    }

    return isLinked;
}

void Shader::apply()
{
    if (program_id != 0 && isLinked)
    {
        glUseProgram(program_id);
    }
}

bool Shader::getUniformLocation(const std::string & uniform_name)
{
    GLint uniform_location = glGetUniformLocation(program_id, uniform_name.c_str());
    
    if (uniform_location != -1)
    {
        uniformsLocations[uniform_name] = uniform_location;
        return true;
    }
    else
    {
        fprintf(stderr, "Error! Can't find uniform %s\n", uniform_name.c_str());
        return false;
    }
}

/* TODO: Use program introspection */
bool Shader::setupUnifomBuffers()
{
    static int binding = 0;

    /* Get number of uniform blocks in a program object. */
    GLint numUniformBlocks;
    glGetProgramiv(program_id, GL_ACTIVE_UNIFORM_BLOCKS, &numUniformBlocks);

    GLint alignment;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);

    /* Get information about each uniform block. */
    for (GLint uniformBlock = 0; uniformBlock < numUniformBlocks; ++uniformBlock)
    {
        UBO * ubo     = new UBO();
        ubo->block_id = uniformBlock;

        GLint nameLength;
        glGetActiveUniformBlockiv(program_id, uniformBlock, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLength);

        std::unique_ptr<GLchar> blockName(new GLchar[nameLength]);
        glGetActiveUniformBlockName(program_id, uniformBlock, nameLength, nullptr, blockName.get());

        GLint blockSize;
        glGetActiveUniformBlockiv(program_id, uniformBlock, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
        ubo->block_size = blockSize;

        GLint numberOfUniformsInBlock;
        glGetActiveUniformBlockiv(program_id, uniformBlock, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numberOfUniformsInBlock);

        std::unique_ptr<GLint> uniformsIndices(new GLint[numberOfUniformsInBlock]);
        glGetActiveUniformBlockiv(program_id, uniformBlock, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniformsIndices.get());
        
        for (int uniformMember = 0; uniformMember < numberOfUniformsInBlock; ++uniformMember)
        {
            if (uniformsIndices.get()[uniformMember] > 0)
            {
                GLuint uniformIndex = uniformsIndices.get()[uniformMember];

                /* Get length of name of uniform variable */
                GLsizei uniformNameLength;
                glGetActiveUniformsiv(program_id, 1, &uniformIndex, GL_UNIFORM_NAME_LENGTH, &uniformNameLength);

                /* Get name of uniform variable */
                std::unique_ptr<GLchar> uniformName(new GLchar[uniformNameLength]);
                glGetActiveUniformName(program_id, uniformIndex, uniformNameLength, nullptr, uniformName.get());
                
                /* Get offset of uniform variable related to start of uniform block */
                GLint uniformOffset;
                glGetActiveUniformsiv(program_id, 1, &uniformIndex, GL_UNIFORM_OFFSET, &uniformOffset);
                
                ubo->uniformMembersOffsets[uniformName.get()] = uniformOffset;
                uniformBlocksMembers[uniformName.get()] = ubo;
            }
        }

        /* Init Uniform Buffer */
        ubo->data = (GLubyte *) malloc(ubo->block_size);

        glGenBuffers(1, &ubo->ubo_id);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo->ubo_id);
        glBufferData(GL_UNIFORM_BUFFER, blockSize, NULL, GL_DYNAMIC_DRAW);
        
        glUniformBlockBinding(program_id, ubo->block_id, binding);
        glBindBufferRange(GL_UNIFORM_BUFFER, binding, ubo->ubo_id, 0, ubo->block_size);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        uniformBlocks.push_back(ubo);

        ++binding;
    }

    return true;
}

void Shader::updateUBOs()
{
    for (auto & ubo : uniformBlocks)
    {
        if (ubo->isDirty)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, ubo->ubo_id);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, ubo->block_size, ubo->data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            ubo->isDirty = false;
        }
    }
}

void Shader::setUniform1f(const std::string & uniformName, float value)
{
    if (uniformsLocations.count(uniformName))
    {
        glProgramUniform1f(program_id, uniformsLocations[uniformName], value);
    }
    else if (uniformBlocksMembers.count(uniformName))
    {
        memcpy(uniformBlocksMembers[uniformName]->data + uniformBlocksMembers[uniformName]->uniformMembersOffsets[uniformName], &value, sizeof(GLfloat));

        uniformBlocksMembers[uniformName]->isDirty = true;
    }
    else
    {
        if (getUniformLocation(uniformName))
        {
            glProgramUniform1f(program_id, uniformsLocations[uniformName], value);
        }
    }
}

void Shader::setUniform1i(const std::string & uniformName, int value)
{
    if (uniformsLocations.count(uniformName))
    {
        glProgramUniform1i(program_id, uniformsLocations[uniformName], value);
    }
    else if (uniformBlocksMembers.count(uniformName))
    {
        memcpy(uniformBlocksMembers[uniformName]->data + uniformBlocksMembers[uniformName]->uniformMembersOffsets[uniformName], &value, sizeof(GLint));

        uniformBlocksMembers[uniformName]->isDirty = true;
    }
    else
    {
        if (getUniformLocation(uniformName))
        {
            glProgramUniform1i(program_id, uniformsLocations[uniformName], value);
        }
    }
}

void Shader::setUniform1ui(const std::string & uniformName, unsigned int value)
{
    if (uniformsLocations.count(uniformName))
    {
        glProgramUniform1ui(program_id, uniformsLocations.at(uniformName), value);
    }
    else if (uniformBlocksMembers.count(uniformName))
    {
        memcpy(uniformBlocksMembers[uniformName]->data + uniformBlocksMembers[uniformName]->uniformMembersOffsets[uniformName], &value, sizeof(GLuint));

        uniformBlocksMembers[uniformName]->isDirty = true;
    }
    else
    {
        if (getUniformLocation(uniformName))
        {
            glProgramUniform1ui(program_id, uniformsLocations[uniformName], value);
        }
    }
}

void Shader::setUniform2fv(const std::string & uniformName, glm::vec2 & vector)
{
    if (uniformsLocations.count(uniformName))
    {
        glProgramUniform2fv(program_id, uniformsLocations[uniformName], 1, glm::value_ptr(vector));
    }
    else if (uniformBlocksMembers.count(uniformName))
    {
        memcpy(uniformBlocksMembers[uniformName]->data + uniformBlocksMembers[uniformName]->uniformMembersOffsets[uniformName], glm::value_ptr(vector), sizeof(glm::vec2));

        uniformBlocksMembers[uniformName]->isDirty = true;
    }
    else
    {
        if (getUniformLocation(uniformName))
        {
            glProgramUniform2fv(program_id, uniformsLocations[uniformName], 1, glm::value_ptr(vector));
        }
    }
}

void Shader::setUniform3fv(const std::string & uniformName, glm::vec3 & vector)
{
    if (uniformsLocations.count(uniformName))
    {
        glProgramUniform3fv(program_id, uniformsLocations[uniformName], 1, glm::value_ptr(vector));
    }
    else if (uniformBlocksMembers.count(uniformName))
    {
        memcpy(uniformBlocksMembers[uniformName]->data + uniformBlocksMembers[uniformName]->uniformMembersOffsets[uniformName], glm::value_ptr(vector), sizeof(glm::vec3));

        uniformBlocksMembers[uniformName]->isDirty = true;
    }
    else
    {
        if (getUniformLocation(uniformName))
        {
            glProgramUniform3fv(program_id, uniformsLocations[uniformName], 1, glm::value_ptr(vector));
        }
    }
}

void Shader::setUniformMatrix3fv(const std::string & uniformName, glm::mat3 & matrix)
{
    if (uniformsLocations.count(uniformName))
    {
        glProgramUniformMatrix3fv(program_id, uniformsLocations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
    }
    else if (uniformBlocksMembers.count(uniformName))
    {
        memcpy(uniformBlocksMembers[uniformName]->data + uniformBlocksMembers[uniformName]->uniformMembersOffsets[uniformName] + 0,  &matrix[0][0], sizeof(glm::vec3));
        memcpy(uniformBlocksMembers[uniformName]->data + uniformBlocksMembers[uniformName]->uniformMembersOffsets[uniformName] + 16, &matrix[1][0], sizeof(glm::vec3));
        memcpy(uniformBlocksMembers[uniformName]->data + uniformBlocksMembers[uniformName]->uniformMembersOffsets[uniformName] + 32, &matrix[2][0], sizeof(glm::vec3));

        uniformBlocksMembers[uniformName]->isDirty = true;
    }
    else
    {
        if (getUniformLocation(uniformName))
        {
            glProgramUniformMatrix3fv(program_id, uniformsLocations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
        }
    }
}

void Shader::setUniformMatrix4fv(const std::string & uniformName, glm::mat4 & matrix)
{
    if (uniformsLocations.count(uniformName))
    {
        glProgramUniformMatrix4fv(program_id, uniformsLocations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
    }
    else if (uniformBlocksMembers.count(uniformName))
    {
        memcpy(uniformBlocksMembers[uniformName]->data + uniformBlocksMembers[uniformName]->uniformMembersOffsets[uniformName], glm::value_ptr(matrix), sizeof(glm::mat4));

        uniformBlocksMembers[uniformName]->isDirty = true;
    }
    else
    {
        if (getUniformLocation(uniformName))
        {
            glProgramUniformMatrix4fv(program_id, uniformsLocations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
        }
    }
}
