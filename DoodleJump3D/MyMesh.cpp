#include "MyMesh.h"

using std::shared_ptr;
using std::make_shared;

void MyMesh::Init(void)
{
    m_bBinded = false;
    m_uVertexCount = 0;

    m_VAO = 0;
    m_VBO = 0;

    m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
    m_pShaderMngr = nullptr;

    if (m_VBO > 0)
        glDeleteBuffers(1, &m_VBO);

    if (m_VAO > 0)
        glDeleteVertexArrays(1, &m_VAO);

    m_lVertex.clear();
    m_lVertexPos.clear();
    m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
    Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
    m_bBinded = other.m_bBinded;

    m_pShaderMngr = other.m_pShaderMngr;

    m_uVertexCount = other.m_uVertexCount;

    m_VAO = other.m_VAO;
    m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
    if (this != &other)
    {
        Release();
        Init();
        MyMesh temp(other);
        Swap(temp);
    }
    return *this;
}
void MyMesh::Swap(MyMesh& other)
{
    std::swap(m_bBinded, other.m_bBinded);
    std::swap(m_uVertexCount, other.m_uVertexCount);

    std::swap(m_VAO, other.m_VAO);
    std::swap(m_VBO, other.m_VBO);

    std::swap(m_lVertex, other.m_lVertex);
    std::swap(m_lVertexPos, other.m_lVertexPos);
    std::swap(m_lVertexCol, other.m_lVertexCol);

    std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
    uint uColorCount = m_lVertexCol.size();
    for (uint i = uColorCount; i < m_uVertexCount; ++i)
    {
        m_lVertexCol.push_back(a_v3Color);
    }
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
    m_lVertexPos.push_back(a_v3Input);
    m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
    m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
    if (m_bBinded)
        return;

    if (m_uVertexCount == 0)
        return;

    CompleteMesh();

    for (uint i = 0; i < m_uVertexCount; i++)
    {
        //Position
        m_lVertex.push_back(m_lVertexPos[i]);
        //Color
        m_lVertex.push_back(m_lVertexCol[i]);
    }
    glGenVertexArrays(1, &m_VAO);//Generate vertex array object
    glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

    glBindVertexArray(m_VAO);//Bind the VAO
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
    glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

    // Color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

    m_bBinded = true;

    glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
    // Use the buffer and shader
    GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
    glUseProgram(nShader);

    //Bind the VAO of this object
    glBindVertexArray(m_VAO);

    // Get the GPU variables by their name and hook them to CPU variables
    GLuint MVP = glGetUniformLocation(nShader, "MVP");
    GLuint wire = glGetUniformLocation(nShader, "wire");

    //Final Projection of the Camera
    matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
    glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));

    //Solid
    glUniform3f(wire, -1.0f, -1.0f, -1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);

    //Wire
    glUniform3f(wire, 1.0f, 0.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.f, -1.f);
    glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
    glDisable(GL_POLYGON_OFFSET_LINE);

    glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
    //C
    //| \
	//A--B
    //This will make the triangle A->B->C 
    AddVertexPosition(a_vBottomLeft);
    AddVertexPosition(a_vBottomRight);
    AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
    //C--D
    //|  |
    //A--B
    //This will make the triangle A->B->C and then the triangle C->B->D
    AddVertexPosition(a_vBottomLeft);
    AddVertexPosition(a_vBottomRight);
    AddVertexPosition(a_vTopLeft);

    AddVertexPosition(a_vTopLeft);
    AddVertexPosition(a_vBottomRight);
    AddVertexPosition(a_vTopRight);
}

void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
    if (a_fSize < 0.01f)
        a_fSize = 0.01f;

    Release();
    Init();

    float fValue = a_fSize * 0.5f;
    //3--2
    //|  |
    //0--1

    vector3 point0(-fValue, -fValue, fValue); //0
    vector3 point1(fValue, -fValue, fValue); //1
    vector3 point2(fValue, fValue, fValue); //2
    vector3 point3(-fValue, fValue, fValue); //3

    vector3 point4(-fValue, -fValue, -fValue); //4
    vector3 point5(fValue, -fValue, -fValue); //5
    vector3 point6(fValue, fValue, -fValue); //6
    vector3 point7(-fValue, fValue, -fValue); //7

    //F
    AddQuad(point0, point1, point3, point2);

    //B
    AddQuad(point5, point4, point6, point7);

    //L
    AddQuad(point4, point0, point7, point3);

    //R
    AddQuad(point1, point5, point2, point6);

    //U
    AddQuad(point3, point2, point7, point6);

    //D
    AddQuad(point4, point5, point0, point1);

    // Adding information about color
    CompleteMesh(a_v3Color);
    CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
    Release();
    Init();

    vector3 v3Value = a_v3Dimensions * 0.5f;
    //3--2
    //|  |
    //0--1
    vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
    vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
    vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
    vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

    vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
    vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
    vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
    vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

    //F
    AddQuad(point0, point1, point3, point2);

    //B
    AddQuad(point5, point4, point6, point7);

    //L
    AddQuad(point4, point0, point7, point3);

    //R
    AddQuad(point1, point5, point2, point6);

    //U
    AddQuad(point3, point2, point7, point6);

    //D
    AddQuad(point4, point5, point0, point1);

    // Adding information about color
    CompleteMesh(a_v3Color);
    CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
    if (a_fRadius < 0.01f)
        a_fRadius = 0.01f;

    if (a_fHeight < 0.01f)
        a_fHeight = 0.01f;

    if (a_nSubdivisions < 3)
        a_nSubdivisions = 3;
    if (a_nSubdivisions > 360)
        a_nSubdivisions = 360;

    Release();
    Init();

    //makes entire cone
    vector3 circleCenter(0.0f, 0.0f, 0.0f);
    vector3 topVertex(0.0f, 0.0f, a_fHeight);
    vector3 next(a_fRadius, 0.0f, 0.0f);
    vector3 prev;
    float factor = (2 * float(PI) / (a_nSubdivisions));
    for (int i = 1; i < a_nSubdivisions + 1; ++i) {
        prev = next;
        float x = a_fRadius * cos((float)i * factor);
        float y = a_fRadius * sin((float)i * factor);
        next = vector3(x, y, 0.0f);
        AddTri(prev, circleCenter, next);
        AddTri(prev, next, topVertex);
    }

    // Adding information about color
    CompleteMesh(a_v3Color);
    CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
    if (a_fRadius < 0.01f)
        a_fRadius = 0.01f;

    if (a_fHeight < 0.01f)
        a_fHeight = 0.01f;

    if (a_nSubdivisions < 3)
        a_nSubdivisions = 3;
    if (a_nSubdivisions > 360)
        a_nSubdivisions = 360;

    Release();
    Init();

    vector3 circleCenterBot(0.0f, -a_fHeight / 2.0f, 0.0f);
    vector3 circleCenterTop(0.0f, a_fHeight / 2.0f, 0.0f);
    vector3 nextT(a_fRadius, a_fHeight / 2.0f, 0.0f);
    vector3 prevT;
    vector3 nextB(a_fRadius, -a_fHeight / 2.0f, 0.0f);
    vector3 prevB;
    float factor = (2 * float(PI) / a_nSubdivisions);
    for (int i = 1; i < a_nSubdivisions + 1; ++i) {
        prevT = nextT;
        prevB = nextB;
        float x = a_fRadius * cos((float)i * factor);
        float z = a_fRadius * sin((float)i * factor);
        nextT = vector3(x, a_fHeight / 2.0f, z);
        nextB = vector3(x, -a_fHeight / 2.0f, z);
        AddTri(prevT, circleCenterTop, nextT);
        AddTri(prevB, nextB, circleCenterBot);
        AddTri(prevB, nextT, nextB);
        AddTri(nextT, prevB, prevT);
    }
    // Adding information about color
    CompleteMesh(a_v3Color);
    CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
    if (a_fOuterRadius < 0.01f)
        a_fOuterRadius = 0.01f;

    if (a_fInnerRadius < 0.005f)
        a_fInnerRadius = 0.005f;

    if (a_fInnerRadius > a_fOuterRadius)
        std::swap(a_fInnerRadius, a_fOuterRadius);

    if (a_fHeight < 0.01f)
        a_fHeight = 0.01f;

    if (a_nSubdivisions < 3)
        a_nSubdivisions = 3;
    if (a_nSubdivisions > 360)
        a_nSubdivisions = 360;

    Release();
    Init();

    float hei = a_fHeight / 2.0f;
    vector3 nextInBot(a_fInnerRadius, 0.0f, -hei);
    vector3 nextOutBot(a_fOuterRadius, 0.0f, -hei);
    vector3 prevInBot;
    vector3 prevOutBot;

    vector3 nextInTop(a_fInnerRadius, 0.0f, hei);
    vector3 nextOutTop(a_fOuterRadius, 0.0f, hei);
    vector3 prevInTop;
    vector3 prevOutTop;
    float factor = (2 * float(PI) / (a_nSubdivisions));
    for (int i = 1; i < a_nSubdivisions + 1; ++i) {
        prevInBot = nextInBot;
        prevOutBot = nextOutBot;
        prevInTop = nextInTop;
        prevOutTop = nextOutTop;

        float xIn = a_fInnerRadius * cos((float)i * factor);
        float yIn = a_fInnerRadius * sin((float)i * factor);
        float xOut = a_fOuterRadius * cos((float)i * factor);
        float yOut = a_fOuterRadius * sin((float)i * factor);

        nextInBot = vector3(xIn, yIn, -hei);
        nextOutBot = vector3(xOut, yOut, -hei);
        nextInTop = vector3(xIn, yIn, hei);
        nextOutTop = vector3(xOut, yOut, hei);

        //makes top and botton base
        AddTri(prevInBot, nextInBot, nextOutBot);
        AddTri(nextOutBot, prevOutBot, prevInBot);
        AddTri(nextInTop, prevInTop, nextOutTop);
        AddTri(prevOutTop, nextOutTop, prevInTop);

        //makes sides
        AddTri(nextInBot, prevInBot, nextInTop);
        AddTri(prevInTop, nextInTop, prevInBot);
        AddTri(prevOutBot, nextOutBot, nextOutTop);
        AddTri(nextOutTop, prevOutTop, prevOutBot);
    }


    // Adding information about color
    CompleteMesh(a_v3Color);
    CompileOpenGL3X();
}
