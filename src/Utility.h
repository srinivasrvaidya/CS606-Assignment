#include<GL/glut.h>


class Utility
{
	public:
	
	void paintString(int ,int ,int , char *);	
	void loadPlyfile(char *);
	void calcNormal(int ,float *, float *, float * );
	void normalise();
};
