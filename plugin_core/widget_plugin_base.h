#ifndef PLUGIN_WIDGET_PLUGIN_BASE_H
#define PLUGIN_WIDGET_PLUGIN_BASE_H

#include <QSettings>

#include "iclock_plugin.h"
#include "iplugin_init.h"

#include "plugin_core_global.h"
#include "widget_plugin_settings.h"

class QGridLayout;

namespace plugin {

class WidgetPluginBasePrivate;

/*!
 * @brief Base class for any 'widget' plugin.
 *
 * This class provides common logic for all 'widget' plugins:
 *   - add/remove plugin widget to/from main window layout
 *   - handle clock settings changes
 *   - render plugin provided text using skin draw engine
 *   - widget autoresize feature
 *   - provide some common settings
 *
 * @see WidgetPluginOption
 */
class PLUGIN_CORE_SHARED_EXPORT WidgetPluginBase : public IClockPlugin,
    public ISettingsPluginInit,
    public IWidgetPluginInit
{
  Q_OBJECT
  Q_INTERFACES(IClockPlugin ISettingsPluginInit IWidgetPluginInit)

public:
  /*! Constructor */
  WidgetPluginBase();
  /*!
   * @brief Init internals with current clock settings.
   *
   * Uses almost all view-related options to configure render.
   * @param current_settings - map with current clock settings
   * @see ISettingsPluginInit, Option, ::skin_draw::SkinDrawer
   */
  virtual void Init(const QMap<Option, QVariant>& current_settings);
  /*!
   * @brief Init widget part of plugin
   *
   * Initializes some variables which will needed for widget initialization.
   * @param main_wnd - clock main window
   */
  virtual void Init(QWidget* main_wnd);

public slots:
  /*!
   * @brief Activate plugin.
   *
   * Creates and adds widget to clock main layout.
   * Starts all plugin activity.
   * @see InitWidget(), IClockPlugin
   */
  virtual void Start();
  /*!
   * @brief Deactivate plugin.
   *
   * Removes created widget from clock layout and destroys it.
   * Stops all plugin activity.
   * @see IClockPlugin
   */
  virtual void Stop();
  /*!
   * @brief Clock settings change listener.
   *
   * This method is handler which will called on every change of any clock option.
   * Almost all view-related options are tracked, they are passed to render.
   * @param option - changed option
   * @param new_value - new option value
   * @see IClockPlugin, Option
   */
  virtual void SettingsListener(Option option, const QVariant& new_value);
  /*!
   * @brief Reimplemented from IClockPlugin::TimeUpdateListener()
   *
   * Used as timer. Redraws widget if needed.
   */
  virtual void TimeUpdateListener();

protected:
  /*!
   * @brief Init plugin specific settings with default values.
   *
   * This function is used during plugin initialization process.
   * It must be reimplemented in child class if needed (plugin has some options).
   * Default implementation does nothing.
   * @param defaults - map to fill with default values
   */
  virtual void InitSettingsDefaults(QSettings::SettingsMap* defaults);

  /*!
   * @brief Create and add to clock @a layout own widget.
   *
   * This function must implement any logic for widget creation and its positioning.
   * This is used during Start().
   * @return created widget
   * @note Widget *must* be created *without* any parent, it will deleted explicitly during Stop().
   * @see Start(), Stop()
   */
  virtual QWidget* InitWidget(QGridLayout* layout) = 0;
  /*!
   * @brief Display rendered @a image on own widget.
   *
   * This function must implement way to display @a image on own created widget.
   * This will called every time when widget redraw will needed or data changed.
   * @see InitWidget(), TimeUpdateListener(), GetWidgetText(), CalculateZoom()
   */
  virtual void DisplayImage(const QImage& image) = 0;
  /*!
   * @brief Get text for rendering.
   *
   * This function will called every time when text for rendering will needed.
   * @return string to render
   * @see TimeUpdateListener()
   */
  virtual QString GetWidgetText() = 0;

  /*!
   * @brief Creates configurator widget.
   *
   * Creates widget with all controls neeed to change common settings and
   * inits this controls to corresponding states.
   * @param parent - parent widget for this widget, usually QDialog or something related
   * @return created widget
   * @see BaseSettingsWidget, WidgetPluginOption
   */
  QWidget* InitConfigWidget(QWidget* parent);

  /*!
   * Calculate rendered image size for given @a text and @a zoom
   * @return calculated image size
   */
  QSize GetImageSize(const QString& text, qreal zoom) const;

  /// plugin name, some meaning string, must be set in child class constructor
  QString plg_name_;
  /// maximum available width for plugin widget
  int avail_width_;

private:
  /*!
   * @brief Calculate widget zoom.
   *
   * Used for 'autoresize feature'. Calculates zoom to fill available width with plugin widget.
   * @param text - text to render
   * @return zoom value
   */
  virtual qreal CalculateZoom(const QString& text) const;

  // use pointer to private data to keep binary compatibility
  WidgetPluginBasePrivate* private_;
  friend class WidgetPluginBasePrivate;
};

} // namespace plugin

#endif // PLUGIN_WIDGET_PLUGIN_BASE_H
