typdef struct Image {
  void (*SetData)(struct Image *self)
  void (*resize)(struct Image *self)
  void (*GetWidth)(struct Image *self)
  void (*GetHeight)(struct Image *self)
  void (*AllocateMemory)(struct Image *self)
  void (*Release)(struct Image *self)
  typedef struct ImageFormat {
    int None = 0;
    int RGBA, RGBA32F;
  } ImageFormat;
} Image;


