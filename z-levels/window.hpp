#include <atomic>
#include <map>
#include <vector>
#include <memory>
#include <unordered_set>
#include <utility>

enum struct Color : uint8_t {
  BLUE,
  RED,
  WHITE,
  GREEN,
  YELLOW
};

class WindowSystem {
private:
  class Window {
    friend class WindowSystem;

  private:
    inline static int window_counter = 0;

    int _x {}, _y {};
    int _width {}, _heigth {};
    int _id;
    bool _opened = true;
    Color _color = Color::BLUE, _bound_color = Color::WHITE;

    Window(int x, int y, int width, int heigth);

    void move(int dx, int dy);
    void resize(int width, int height);

    void set_color(const std::string &color);
    void set_bound_color(const std::string &color);

    int get_id();

    bool is_intersect(int x, int y);

    void close();
    void open();

    static Color parse_color(const std::string &color);
  };

  constexpr static int _screen_width = 40;
  constexpr static int _screen_heigth = 20;

  struct Pixel {
    Color color;
    char ch;
  };

  // std::array
  using Screen = Pixel[_screen_width * _screen_heigth];

  using WindowContainer = std::vector<std::shared_ptr<Window>>;
  using WindowIter = WindowContainer::iterator;

  Screen _screen;

  WindowContainer _windows;
  std::unordered_set<int> _windows_ids;

private:
  WindowSystem();

public:
  static WindowSystem & get();

  // rule of 5
  WindowSystem(WindowSystem &other) = delete;
  WindowSystem & operator=(WindowSystem &other) = delete;

  int create_window(int x, int y, int width, int heigth);
  void delete_window(int id);

  const std::unordered_set<int> & get_windows_ids();

  void move_window(int id, int dx, int dy);
  void resize_window(int id, int width, int height);
  void close_window(int id);
  void open_window(int id);
  void set_window_color(int id, const std::string &color);
  void set_window_bound_color(int id, const std::string &color);

  void click(int x, int y);

  void draw();

private:
  void assert_window_exists(int id);
  WindowIter find_window_iter(int id);
  void push_on_top(WindowIter wnd);
  void push_on_bottom(WindowIter wnd);

  void clear_screen();
  void set_pixel(int x, int y, char ch, Color color);

  void draw_screen() const;
  void putchar_on_pixel(const Pixel &pixel) const;
};