/*

  Copyright (C) 2017 Gonzalo José Carracedo Carballal

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this program.  If not, see
  <http://www.gnu.org/licenses/>

*/

#ifndef _GUI_GUI_H
#define _GUI_GUI_H

#include <sigutils/sigutils.h>
#include <suscan.h>

#include <gtk/gtk.h>

#include "spectrum.h"
#include "inspector.h"

#ifndef PKGDATADIR
#define PKGDATADIR "/usr"
#endif

#define SUSCAN_GUI_SETTINGS_ID "org.actinid.SUScan"
#define SUSCAN_GUI_MAX_CHANNELS 10

struct suscan_gui_cfgui {
  suscan_config_t *config; /* Borrowed pointer */
  PTR_LIST(GtkWidget, widget);
  GtkGrid *grid;
};

struct suscan_gui_src_ui {
  const struct suscan_source *source;
  struct suscan_source_config *config;
  struct suscan_gui_cfgui *cfgui;
};

enum suscan_gui_state {
  SUSCAN_GUI_STATE_STOPPED,
  SUSCAN_GUI_STATE_RUNNING,
  SUSCAN_GUI_STATE_RESTARTING,
  SUSCAN_GUI_STATE_STOPPING,
  SUSCAN_GUI_STATE_QUITTING
};

struct suscan_gui;

struct suscan_gui_recent {
  struct suscan_gui *gui;
  char *conf_string;
  struct suscan_source_config *config;
};

struct suscan_gui {
  /* Application settings */
  GSettings *settings;

  /* Widgets */
  GtkBuilder *builder;
  GtkWindow *main;
  GtkLabel *titleLabel;
  GtkLabel *subTitleLabel;
  GtkButton *toggleConnect;
  GtkButton *preferencesButton;
  GtkListStore *sourceListStore;
  GtkListStore *channelListStore;
  GtkDialog *settingsDialog;
  GtkDialog *aboutDialog;
  GtkGrid *sourceGrid;
  GtkComboBox *sourceCombo;
  GtkHeaderBar *headerBar;
  GtkMenuBar *menuBar;
  GtkLabel *freqLabel;
  GObject *sourceAlignment;
  GtkMenu *recentMenu;
  GtkMenuItem *emptyMenuItem;

  GtkRadioMenuItem    *spectrogramMenuItem;
  GtkRadioMenuItem    *waterfallMenuItem;
  GtkToggleToolButton *overlayChannelToggleButton;
  GtkToggleToolButton *autoGainToggleButton;
  GtkScaleButton      *gainScaleButton;
  GtkScaleButton      *rangeScaleButton;

  GtkTreeViewColumn *centerFrequencyCol;
  GtkTreeViewColumn *snrCol;
  GtkTreeViewColumn *signalLevelCol;
  GtkTreeViewColumn *noiseLevelCol;
  GtkTreeViewColumn *bandwidthCol;

  GtkCellRendererText *centerFrequencyCellRenderer;
  GtkCellRendererText *snrCellRenderer;
  GtkCellRendererText *signalLevelCellRenderer;
  GtkCellRendererText *noiseLevelCellRenderer;
  GtkCellRendererText *bandwidthCellRenderer;

  GtkLevelBar *cpuLevelBar;
  GtkLabel *cpuLabel;

  GtkLevelBar *n0LevelBar;
  GtkLabel *n0Label;

  GtkLabel *spectrumSampleRate;
  GtkLabel *spectrumDbsPerDivLabel;
  GtkLabel *spectrumRefLevelLabel;
  GtkLabel *spectrumFreqScaleLabel;
  GtkLabel *spectrumFreqOffsetLabel;

  GtkMenu *channelMenu;
  GtkMenuItem *channelHeaderMenuItem;
  GtkMenuItem *openInspectorMenuItem;

  GtkNotebook *analyzerViewsNotebook;

  GtkTreeView *logMessagesTreeView;
  GtkListStore *logMessagesListStore;

  /* Settings widgets */
  GtkEntry *alphaEntry;
  GtkEntry *betaEntry;
  GtkEntry *gammaEntry;
  GtkEntry *snrEntry;

  GtkEntry *bufSizeEntry;
  GtkEntry *chIntervalEntry;
  GtkEntry *psdIntervalEntry;

  GtkRadioButton *rectangularWindowButton;
  GtkRadioButton *hammingWindowButton;
  GtkRadioButton *hannWindowButton;
  GtkRadioButton *blackmannHarrisWindowButton;
  GtkRadioButton *flatTopWindowButton;

  /* GUI state */
  enum suscan_gui_state state;

  /* Analyzer integration */
  struct suscan_source_config *analyzer_source_config;
  struct suscan_analyzer_params analyzer_params;
  suscan_analyzer_t *analyzer;
  struct suscan_mq mq_out;
  GThread *async_thread;

  /* Main spectrum */
  SUSCOUNT current_samp_rate;
  struct sigutils_channel selected_channel;
  struct suscan_gui_spectrum main_spectrum;

  /* Keep list of inspector tabs */
  PTR_LIST(suscan_gui_inspector_t, inspector);

  /* Keep a list of the last configurations used */
  PTR_LIST(struct suscan_gui_recent, recent);
};

typedef struct suscan_gui suscan_gui_t;

void suscan_gui_destroy(suscan_gui_t *gui);

suscan_gui_t *suscan_gui_new(int argc, char **argv);

SUBOOL suscan_gui_start(
    int argc,
    char **argv,
    struct suscan_source_config **config_list,
    unsigned int config_count);

void suscan_gui_msgbox(
    suscan_gui_t *gui,
    GtkMessageType type,
    const char *title,
    const char *fmt,
    ...);

void suscan_gui_setup_logging(suscan_gui_t *gui);

/* Generic UI functions */
void suscan_gui_text_entry_set_float(GtkEntry *entry, SUFLOAT value);

void suscan_gui_text_entry_set_scount(GtkEntry *entry, SUSCOUNT value);

void suscan_gui_text_entry_set_integer(GtkEntry *entry, int64_t value);

/* Generic configuration UI */
SUBOOL suscan_gui_cfgui_parse(struct suscan_gui_cfgui *ui);

void suscan_gui_cfgui_dump(struct suscan_gui_cfgui *ui);

void suscan_gui_cfgui_destroy(struct suscan_gui_cfgui *ui);

GtkWidget *suscan_gui_cfgui_get_root(const struct suscan_gui_cfgui *ui);

struct suscan_gui_cfgui *suscan_gui_cfgui_new(suscan_config_t *config);

/* Source UI API */
struct suscan_gui_src_ui *suscan_gui_get_selected_src_ui(
    const suscan_gui_t *gui);

SUBOOL suscan_gui_set_selected_src_ui(
    suscan_gui_t *gui,
    const struct suscan_gui_src_ui *ui);

void suscan_gui_src_ui_to_dialog(const struct suscan_gui_src_ui *ui);

SUBOOL suscan_gui_src_ui_from_dialog(struct suscan_gui_src_ui *ui);

SUBOOL suscan_gui_set_title(suscan_gui_t *gui, const char *title);

void suscan_gui_set_src_ui(
    suscan_gui_t *gui,
    struct suscan_gui_src_ui *ui);

/* Analyzer params API */
void suscan_gui_analyzer_params_to_dialog(suscan_gui_t *gui);

SUBOOL suscan_gui_analyzer_params_from_dialog(suscan_gui_t *gui);

/* GUI State */
void suscan_gui_update_state(
    suscan_gui_t *gui,
    enum suscan_gui_state state);

void suscan_gui_detach_all_inspectors(suscan_gui_t *gui);

SUBOOL suscan_gui_connect(suscan_gui_t *gui);
void suscan_gui_reconnect(suscan_gui_t *gui);
void suscan_gui_disconnect(suscan_gui_t *gui);
void suscan_gui_quit(suscan_gui_t *gui);

/* Some message dialogs */
#define suscan_error(gui, title, fmt, arg...) \
    suscan_gui_msgbox(gui, GTK_MESSAGE_ERROR, title, fmt, ##arg)

#define suscan_warning(gui, title, fmt, arg...) \
    suscan_gui_msgbox(gui, GTK_MESSAGE_WARNING, title, fmt, ##arg)

/* Main GUI inspector list handling methods */
SUBOOL suscan_gui_remove_inspector(
    suscan_gui_t *gui,
    suscan_gui_inspector_t *insp);

SUBOOL suscan_gui_add_inspector(
    suscan_gui_t *gui,
    suscan_gui_inspector_t *insp);

suscan_gui_inspector_t *suscan_gui_get_inspector(
    const suscan_gui_t *gui,
    uint32_t inspector_id);

/* Source API */
struct suscan_gui_src_ui *suscan_gui_lookup_source_config(
    const suscan_gui_t *gui,
    const struct suscan_source *src);

/* Recent source configuration management */
SUBOOL suscan_gui_append_recent(
    suscan_gui_t *gui,
    const struct suscan_source_config *config);

struct suscan_gui_recent *suscan_gui_recent_new(
    suscan_gui_t *gui,
    char *conf_string);

void suscan_gui_recent_destroy(struct suscan_gui_recent *recent);

void suscan_gui_retrieve_recent(suscan_gui_t *gui);

void suscan_gui_store_recent(suscan_gui_t *gui);

void suscan_gui_retrieve_analyzer_params(suscan_gui_t *gui);

void suscan_gui_store_analyzer_params(suscan_gui_t *gui);

#endif /* _GUI_GUI_H */
