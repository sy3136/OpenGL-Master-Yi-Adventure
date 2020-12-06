#pragma once
#ifndef __TREE__
#define __TREE__
class Tree
{
public:
	Tree() {}
	Tree(mesh2* pMesh, vec3 scale, vec3 pos) {
		this->pMesh = pMesh;
		this->scale = scale;
		this->pos = pos;
	}

	vec3 scale;
	vec3 pos;
	mesh2* pMesh;
	mat4 model_matrix;

	void update() {
		mat4 tree_scale_matrix = 
		{
			scale.x, 0, 0, 0,
			0, scale.y, 0, 0,
			0, 0, scale.z, 0,
			0, 0, 0, 1
		};
		mat4 tree_translate_matrix = 
		{
			1, 0, 0, pos.x,
			0, 1, 0, pos.y,
			0, 0, 1, pos.z,
			0, 0, 0, 1
		};

		model_matrix = tree_translate_matrix * tree_scale_matrix;
	}

	void render(GLuint program, int mode) {
		for (size_t k = 0, kn = pMesh->geometry_list.size(); k < kn; k++) {
			geometry& g = pMesh->geometry_list[k];
			glUniform1i(glGetUniformLocation(program, "is_model"), 1);
			glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_TRUE, model_matrix);
			glUniform1i(glGetUniformLocation(program, "alphablending"), 0);
			glUniform1i(glGetUniformLocation(program, "bump"), 0);
			glUniform1i(glGetUniformLocation(program, "mode"), 0);
			if (g.mat->textures.diffuse) {
				glBindTexture(GL_TEXTURE_2D, g.mat->textures.diffuse->id);
				glUniform1i(glGetUniformLocation(program, "TEX"), 0);	 // GL_TEXTURE0
				glUniform1i(glGetUniformLocation(program, "use_texture"), true);
			}

			else {
				glUniform4fv(glGetUniformLocation(program, "diffuse"), 1, (const float*)(&g.mat->diffuse));
				glUniform1i(glGetUniformLocation(program, "use_texture"), false);
			}

			// render vertices: trigger shader programs to process vertex data
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->index_buffer);
			glDrawElements(GL_TRIANGLES, g.index_count, GL_UNSIGNED_INT, (GLvoid*)(g.index_start * sizeof(GLuint)));
		}
		glUniform1i(glGetUniformLocation(program, "is_model"), 0);
	}

};

#endif