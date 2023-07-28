# xQt

QT extensions for xlabMedical

## Usage in project

**xQt** can be included as a subdirectory. Given that your
project is named `my_project` and you wish to *public* 
link to **xQt**, add the following lines to your `CMakeLists.txt`

```cmake
add_subdirectory(xQt)
target_link_libraries(my_project PUBLIC xQt::xQt)
```

**NOTE** that your `CMakeLists.txt` must be in the parent directory.

## Docs

* [xQt::callback_dispatcher](#xqtcallback_dispatcher)
* [xQt::callback_respondable<Derived, WidgetBase>](#xqtcallback_respondablederived-widgetbase)
* [xQt::post_callback](#xqtpost_callback)
* [xQt::send_callback](#xqtsend_callback)
* [xQt::event_dispatcher<_QObject>](#xqtevent_dispatcher_qobject)

### xQt::callback_dispatcher

An object that lives on the main thread, and is capable of executing callbacks. The callbacks can be dispatched through it on any thread and they will execute on the main thread.

#### Methods

---

```c++
template <typename F>
auto dispatch(F &&f, int priority = Qt::NormalEventPriority) -> void;
```

Dispatches to callback `F` (taking no arguments) to be executed on the main thread.

---

```c++
template <typename F>
auto operator()(F &&f, int priority = Qt::NormalEventPriority) -> void;
```

Dispatches to callback `F` (taking no arguments) to be executed on the main thread.

---

#### Example

```c++
xQt::callback_dispatcher cd;

// on some other thread
cd([...](){
   // this will execute on the main thread
});
// or
cd.dispatch([...](){
   // this will execute on the main thread
});
```

### xQt::callback_respondable<Derived, WidgetBase>

Template endows a QWidget `T` with the ability to be posted and
sent callbacks in a **thread-safe** manner.

Assume you have a widget named `xOpenGLWidget`:

```c++
class xOpenGLWidget : public SomeWidget {
  /* ....
   * implementation
   * *...
   */
};
``` 

#### Integration

When you are implementing your own widget, change the definition to inherit from `xQt::callback_respondable<xOpenGLWidget, SomeWidget>` instead of `SomeWidget`.

```c++
class xOpenGLWidget : public xQt::callback_respondable<xOpenGLWidget, SomeWidget> {
  /* ....
   * implementation
   * *...
   */
};

```

**NOTE** that if
you implement `customEvent` method in the parent (e.g. 
`xOpenGLWidget`) for some other custom event, you
**must** forward the event, when it is not of your own
type (but you should always do that anyhow :) )


All existing code will still work, but now `xOpenGLWidget` can
be posted with callback events.

#### Added methods

---

```c++
auto post_callback(F &&f, int priority = Qt::NormalEventPriority) -> void
```

where `F` is any callable object taking `Qobject*` as its first argument. **NOTE** that this method **is** thread-safe.

---

```c++
auto send_callback(F &&f) -> void
```

where `F` is any callable object taking `Qobject*` as its first argument. **NOTE** that this method **is not** thread-safe.

---

```c++
auto make_event_dispatcher() -> event_dispatcher<Derived>;
```

Returns an instance of [xQt::event_dispatcher<QObject>](#xqtevent_dispatcher_qobject) for the `Derived`class.

---

#### Example

```c++
xOpenGLWidget *w; // = new ...;

// in a different (non-GUI) thread
vtkPolyData *poly = //...;
auto callback = [poly](xOpenGLWidget *) {
  // process poly in some way
  obj->Render();
};
w->post_callback(callback);
```


### xQt::post_callback

```c++
template <typename F>
auto post_callback(QObject *q_object, F &&f,
                   int priority = Qt::NormalEventPriority) -> void;
```

Posts a callback to `q_object`. It only makes sense when `Qobject` was casted from `callback_respondable<T>`. **NOTE** that this method **is** thread-safe.

### xQt::send_callback

```c++
template <typename F> auto send_callback(QObject *q_object, F &&f) -> void;
```

Sends a callback to `q_object`. It only makes sense when `Qobject` was casted from `callback_respondable<T>`. **NOTE** that this method **is not** thread-safe.

### xQt::event_dispatcher<_QObject>

Eases dispatching events and callbacks from different classes.

#### Members

```c++
_QObject * _object
```

where `_object` is a pointer to the object to which events will be dispatched.

#### Methods

---

```c++
auto set_object(_QObject *q_object) -> void;
```

Sets the object to which to dispatch events.

---

```c++
auto post(QEvent *q_event, int priority = Qt::NormalEventPriority) -> void;
```

Posts an event to `q_object` member. It only makes sense when `Qobject` was casted from `callback_respondable<T>`. **NOTE** that this method **is** thread-safe.

---

```c++
auto send(QEvent *q_event) -> void;
```

Sends an event to `q_object` member. It only makes sense when `Qobject` was casted from `callback_respondable<T>`. **NOTE** that this method **is not** thread-safe.

---

```c++
template <typename F>
auto post_callback(F &&f, int priority = Qt::NormalEventPriority) -> void
```

Posts a callback to `q_object` member. It only makes sense when `Qobject` was casted from `callback_respondable<T>`. **NOTE** that this method **is** thread-safe.

---

```c++
template <typename F> auto send_callback(F &&f) -> void
```

Sends an event to `q_object` member. It only makes sense when `Qobject` was casted from `callback_respondable<T>`. **NOTE** that this method **is not** thread-safe.

---

#### Example

```c++
xOpenGLWidget *w; // = new ...;

// deeper in codebase

class worker_t {
  xQt::event_dispatcher<xOpenGLWidget> ed;
  // ....
  void do_work0() {
    // do work in a different thread
    // post a callback
    ed.post_callback(
    [](xOpenGLWidget *obj){
       // ...
       obj->Render();
    });
  }
    void do_work1() {
    // do work in a different thread
    // post a an event
    ed.post(new QResizeEvent({1, 1}, {1, 1}));
  }
};

worker_t worker;
worker.ed = w->make_event_dispatcher();
// on a different thread
worker.do_work0();
worker.d
```

---
