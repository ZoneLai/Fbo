#include "Common.h"
#include "TriangleManager.h"
#include "GLUtil.h"
#include "shader/camera.frag"
#include "shader/camera.vert"
/* GL_OES_EGL_image_external */
#ifndef GL_OES_EGL_image_external
#define GL_TEXTURE_EXTERNAL_OES                                 0x8D65
#endif
template<> TriangleManager* Singleton<TriangleManager>::msSingleton = nullptr;
TriangleManager::TriangleManager()
    : _widgetWidth(0)
    , _widgetHeight(0)
    , _photoWidth(0)
    , _photoHeight(0)
    , _sProgramPlay(0)
    , _vaoId(0)
    , _vboBuffer(0)
    , _frameBuffer(0)
    , _renderBuffer(0)
    , _textures{0}
    , _positionLoc(-1)
    , _textCoordLoc(-1)
    , _mvpMatrixLoc(-1)
    , _sampler2DLoc(-1)
    , _mvpMatrix(glm::mat4(1.0f))
    , _modelMatrix(glm::mat4(1.0f))
    , _viewMatrix(glm::mat4(1.0f))
    , _projectionMatrix(glm::mat4(1.0f))
    , _bmpBuffer(nullptr)
{
}

TriangleManager::~TriangleManager() {
}

void TriangleManager::onCreate() {
}

void TriangleManager::onResume() {
}

void TriangleManager::onPause() {
}

void TriangleManager::onStop() {
}

void TriangleManager::onDestroy() {
    glDeleteVertexArrays(1, &_vaoId);
    glDeleteBuffers(1, &_vboBuffer);
    glDeleteTextures(2, _textures);
    glDeleteProgram(_sProgramPlay);
    if (_bmpBuffer != nullptr) {
        delete[] _bmpBuffer;
        _bmpBuffer = nullptr;
    }
}

void TriangleManager::initGL(int widgetWidth, int widgetHeight, int photoWidth, int photoHeight) {
    _widgetWidth            = widgetWidth;
    _widgetHeight           = widgetHeight;
    _photoWidth             = photoWidth;
    _photoHeight            = photoHeight;
    if (CompileShaderProgram(camera_play_vert, camera_play_frag, &_sProgramPlay)) {
        _positionLoc	    = glGetAttribLocation(_sProgramPlay,    "a_Position");
        _textCoordLoc		= glGetAttribLocation(_sProgramPlay,    "a_TextCoord");
        _mvpMatrixLoc       = glGetUniformLocation(_sProgramPlay,   "u_MvpMatrix");
        _sampler2DLoc       = glGetUniformLocation(_sProgramPlay,   "u_Texture");
        GLfloat vertices[]  = {
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // 1
             0.5f, -0.5f, 0.0f, 1.0f, 1.0f, // 2
            -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, // 3
            -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, // 3
             0.5f, -0.5f, 0.0f, 1.0f, 1.0f, // 2
             0.5f,  0.5f, 0.0f, 1.0f, 0.0f  // 4
        };
        glGenVertexArrays(1, &_vaoId);
        glBindVertexArray(_vaoId);
        glGenBuffers(1, &_vboBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _vboBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 顶点坐标
        glVertexAttribPointer(
            _positionLoc,
            3,
            GL_FLOAT,
            GL_FALSE,
            5 * sizeof(GL_FLOAT),
            (GLvoid*)0
        );
        glEnableVertexAttribArray(_positionLoc);
        // 纹理坐标
        glVertexAttribPointer(
            _textCoordLoc,
            2,
            GL_FLOAT,
            GL_FALSE,
            5 * sizeof(GL_FLOAT),
            (GLvoid*)(3 * sizeof(GL_FLOAT))
        );
        glEnableVertexAttribArray(_textCoordLoc);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        // fbo && rbo
        glGenFramebuffers(1, &_frameBuffer);
        glGenRenderbuffers(1, &_renderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _photoWidth,_photoHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _renderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // texture[0]
        glGenTextures(2, _textures);
        glBindTexture(GL_TEXTURE_2D, _textures[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // texImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, mBitmap, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        // texture[1]
        glBindTexture(GL_TEXTURE_2D, _textures[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _photoWidth,_photoHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    } else {
        LOGE("CompileShaderProgram===================");
    }
}

void TriangleManager::drawFrame() {
    LOGE("===========================");
    glViewport(0, 0, _widgetWidth, _widgetHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textures[1], 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _renderBuffer);

    glBindVertexArray(_vaoId);
    glUseProgram(_sProgramPlay);
    glUniformMatrix4fv(_mvpMatrixLoc, 1, GL_FALSE, glm::value_ptr(_mvpMatrix));
    // 启用纹理单元 绑定纹理对象
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textures[0]);
    glUniform1i(_sampler2DLoc, 0);      // 设置纹理单元为0号
    glDrawArrays(GL_TRIANGLES, 0, 6);   // 顶点个数为6
    glBindVertexArray(0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void TriangleManager::onChange(int widgetWidth, int widgetHeight) {
    _widgetWidth        = widgetWidth;
    _widgetHeight       = widgetHeight;
    _modelMatrix	    = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    _viewMatrix         = glm::lookAt(glm::vec3(0.0f, 0.0f, 6.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    _projectionMatrix   = glm::ortho(-1.0f, 1.0f, -(float) widgetHeight / widgetWidth, (float) widgetHeight / widgetWidth, 5.0f, 7.0f);
    _mvpMatrix		    = _projectionMatrix * _viewMatrix * _modelMatrix;
}

void TriangleManager::setAssetsBmp(AAssetManager* mgr,  const char* fileName) {
    AAsset* asset       = AAssetManager_open(mgr, fileName, AASSET_MODE_UNKNOWN);
    off_t fileSize      = AAsset_getLength(asset);
    uint8_t* buffer     = new uint8_t[fileSize];
    int numBytesRead    = AAsset_read(asset, buffer, fileSize);
    LOGE("numBytesRead = %d", numBytesRead);
    if (_bmpBuffer != nullptr) {
        delete[] _bmpBuffer;
        _bmpBuffer = nullptr;
    }
    _bmpBuffer = new uint8_t[fileSize];
    memcpy(_bmpBuffer, buffer, fileSize);
    delete[] buffer;
    buffer = nullptr;
    AAsset_close(asset);
}

GLint TriangleManager::getCameraTextureId() {
    return _textures[0];
}