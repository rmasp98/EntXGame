

#include "common/shader.hpp"




GLuint LoadShaders(std::vector<std::string> filePaths) {

	GLint result = GL_FALSE;
	GLint infoLogLength;
	std::vector<GLuint> ids;
	GLuint programID = glCreateProgram();

	GLuint vertInd = 0, geomInd = 0, fragInd = 0;

	//Loop over each shader
	for (GLuint iShader=0; iShader < filePaths.size(); iShader++) {
		//Determine which tyoe of shader
		GLuint shaderID = 0;
		if ((filePaths[iShader].compare(filePaths[iShader].size() - 3, 3, ".vs") == 0) && (!vertInd++))
			shaderID = glCreateShader(GL_VERTEX_SHADER);
		else if ((filePaths[iShader].compare(filePaths[iShader].size() - 3, 3, ".gs") == 0) && (!geomInd++))
			shaderID = glCreateShader(GL_GEOMETRY_SHADER);
		else if ((filePaths[iShader].compare(filePaths[iShader].size() - 3, 3, ".fs") == 0) && (!fragInd++))
			shaderID = glCreateShader(GL_FRAGMENT_SHADER);
		else {
			std::cerr << "Shaders do not have the extension: .vs, .gs, or .fs; or two of the same shader is loaded" << std::endl;
			return 0;
		}

		// Read code from shader
		std::string shaderCode;
		std::ifstream shaderStream(filePaths[iShader].c_str(), std::ios::in);
		if (shaderStream.is_open()) {
			std::string line = "";
			while(getline(shaderStream, line))
				shaderCode += "\n" + line;

			shaderStream.close();
		} else {
			std::cerr << "Impossible to open " << filePaths[iShader] << ". Are you in the right directory? Don't forget to read the FAQ!" << std::endl;
			return 0;
		}


		// Compile shader
		std::cout << "Compiling shader: " << filePaths[iShader] << std::endl;
		char const * sourcePointer = shaderCode.c_str();
		glShaderSource(shaderID, 1, &sourcePointer , NULL);
		glCompileShader(shaderID);

		// Check shader
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {
			GLchar* shaderErrorMessage = new GLchar[infoLogLength+1];
			glGetShaderInfoLog(shaderID, infoLogLength, NULL, shaderErrorMessage);
			std::cout << shaderErrorMessage << std::endl;
			delete[] shaderErrorMessage;
		}

		//Attach shader and add to array from later deletion
		glAttachShader(programID, shaderID);
		ids.push_back(shaderID);
	}

	if (vertInd && fragInd) {
		// Link the program
		std::cout << "Linking program" << std::endl;
		glLinkProgram(programID);

		// Error check the linking
		glGetProgramiv(programID, GL_LINK_STATUS, &result);
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {
			GLchar* programErrorMessage = new GLchar[infoLogLength+1];
			glGetProgramInfoLog(programID, infoLogLength, NULL, programErrorMessage);
			std::cout << programErrorMessage << std::endl;
		}

		// Delete the shaders
		for (GLuint iShader=0; iShader < ids.size(); iShader++)
			glDeleteShader(ids[iShader]);

		return programID;
	} else {
		std::cerr << "You have not loaded at least a vertex and fragment shader" << std::endl;
		return 0;
	}
}
