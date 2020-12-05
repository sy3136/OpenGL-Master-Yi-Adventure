#pragma once
#ifndef __TREE__
#define __TREE__
class Tree
{
public:
	Tree() {}
	Tree(mesh2* pMesh, vec3 scale, vec3 translate) {
		this->pMesh = pMesh;
		this->scale = scale;
		this->translate = translate;
	}

	vec3 scale;
	vec3 translate;
	mesh2* pMesh;
	mat4 tree_matrix;

	void update(GLuint program) {

		mat4 tree_matrix = mat4();
		mat4 tree_scale_matrix = mat4::scale(scale);
		mat4 tree_translate_matrix = mat4::translate(this->translate.x, this->translate.y, 1.0f);

		tree_matrix = tree_translate_matrix * tree_scale_matrix * tree_matrix;
		GLint uloc;
		
		uloc = glGetUniformLocation(program, "tree_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, tree_matrix);

	}

	void render(GLuint program) {

		glUniform1i(glGetUniformLocation(program, "is_model"), 1);
		glBindVertexArray(pMesh->vertex_array);
		for (size_t k = 0, kn = pMesh->geometry_list.size(); k < kn; k++) {
			geometry& g = pMesh->geometry_list[k];
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