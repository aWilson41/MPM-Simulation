#include "PolyDataMapper.h"
#include "PolyData.h"
#include "Renderer.h"
#include <gl/glew.h>

void PolyDataMapper::draw(Renderer* ren)
{
	if (polyData == nullptr || vboID == -1)
		return;

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Triangle) / 3.0, (void*)0);
	glEnableVertexAttribArray(0);

	// Tex coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(Triangle) / 3.0, (void*)0);
	glEnableVertexAttribArray(0);

	// Normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Triangle) / 3.0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(polyData->GetNumOfPoints()));

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}