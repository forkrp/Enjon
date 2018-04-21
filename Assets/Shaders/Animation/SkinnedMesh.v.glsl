#version 330 core

layout (location = 0) in vec3 aVertexPosition;
layout (location = 1) in vec3 aVertexNormal;
layout (location = 2) in vec3 aVertexTangent;
layout (location = 3) in vec2 aVertexUV;
layout (location = 4) in ivec4 aJointIDs;
layout (location = 5) in vec4 aJointWeights;

const int MAX_BONES = 100;

out VS_OUT
{
	mat3 TBN;
	vec4 ObjectID;
	vec4 PreviousFragPositionClipSpace;
	vec4 CurrentFragPositionClipSpace;
} vs_out;

// Gloabl Uniforms
uniform mat4 uViewProjection;
uniform mat4 uPreviousViewProjection;
uniform vec3 uViewPositionWorldSpace;
uniform mat4 uModel = mat4( 1.0f );
uniform mat4 uPreviousModel = mat4( 1.0f );
uniform vec4 uObjectID;
uniform mat4 uJointTransforms[MAX_BONES];

// Vertex Main
void main()
{
	// Calculate joint transform
	mat4 jointTransform = uJointTransforms[aJointIDs[0]] * aJointWeights[0];
	jointTransform += uJointTransforms[aJointIDs[1]] * aJointWeights[1];
	jointTransform += uJointTransforms[aJointIDs[2]] * aJointWeights[2];
	jointTransform += uJointTransforms[aJointIDs[3]] * aJointWeights[3];

	vec4 posL = jointTransform * vec4(aVertexPosition, 1.0);
	vec3 worldPosition = ( uModel * posL ).xyz;
	gl_Position = uViewProjection * vec4( worldPosition, 1.0 );

	vec4 normalL = jointTransform * vec4(aVertexNormal, 0.0);
	vec3 N = normalize( mat3(uModel) * normalL );
	vec3 T = normalize( mat3(uModel) * aVertexTangent );
	// Reorthogonalize with respect to N
	T = normalize( T - dot(T, N) * N );

	// Calculate Bitangent
	vec3 B = cross( N, T );

	// TBN
	mat3 TBN = mat3( T, B, N );

	// TS_TBN
	mat3 TS_TBN = transpose( TBN );

	// Output Vertex Data
	vs_out.FragPositionWorldSpace = worldPosition;
	vs_out.TexCoords = vec2( aVertexUV.x, -aVertexUV.y );
	vs_out.ViewPositionTangentSpace = TS_TBN * uViewPositionWorldSpace;
	vs_out.FragPositionTangentSpace = TS_TBN * vs_out.FragPositionWorldSpace;
	vs_out.CurrentFragPositionClipSpace = gl_Position;
	vs_out.PreviousFragPositionClipSpace = uPreviousViewProjection * uPreviousModel * vec4( aVertexPosition, 1.0 );
	vs_out.TBN = TBN;
	vs_out.TS_TBN = TS_TBN;
	vs_out.ObjectID = uObjectID;
}