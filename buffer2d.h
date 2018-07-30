class Buffer2D {
public:
	int width, height;
	float *buffer;

	Buffer2D(int width, int height);
	~Buffer2D();

	void clear(float val);

	float get(int x, int y);
	void set(int x, int y, float val);
};

Buffer2D::Buffer2D(int width, int height)
{
	this->width = width;
	this->height = height;

	this->buffer = (float *)malloc(sizeof(float) * width * height);
}

Buffer2D::~Buffer2D()
{
	free(this->buffer);
}

void Buffer2D::clear(float val)
{
	for (int i = 0; i < this->width * this->height; i++) {
		this->buffer[i] = val;
	}
}

inline float Buffer2D::get(int x, int y)
{
	return (this->buffer[y * this->width + x]);
}

inline void Buffer2D::set(int x, int y, float val)
{
	this->buffer[y * this->width + x] = val;
}
