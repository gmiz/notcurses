#include "demo.h"

static int centercols;

static int
animate(struct notcurses* nc, struct ncplane* ncp, void* curry){
  int* flipmode = curry;
  int rows, cols;
  ncplane_dim_yx(ncp, &rows, &cols);
  const bool smallscreen = rows < 26;
  const int row1 = rows - 10 + smallscreen;
  const int row2 = 6 - smallscreen; // box always starts on 4; don't stomp riser
  int startx = (cols - (centercols - 2)) / 2;
  ncplane_set_fg_rgb8(ncp, 0xd0, 0xf0, 0xd0);
  for(int x = startx ; x < startx + centercols - 2 ; ++x){
    if(ncplane_putwc_yx(ncp, row1, x, x % 2 != *flipmode % 2 ? L'◪' : L'◩') <= 0){
      return -1;
    }
    if(ncplane_putwc_yx(ncp, row2, x, x % 2 == *flipmode % 2 ? L'◪' : L'◩') <= 0){
      return -1;
    }
  }
  ++*flipmode;
  int err;
  if( (err = demo_render(nc)) ){
    return err;
  }
  return 0;
}

static int
greatscott(struct notcurses* nc){
  char* path = find_data("greatscott.jpg");
  if(path == NULL){
    return -1;
  }
  struct ncvisual* ncv = ncvisual_from_file(path);
  free(path);
  if(ncv == NULL){
    return -1;
  }
  struct ncvisual_options vopts = {
    .y = 1,
    .x = NCALIGN_CENTER,
    .blitter = NCBLIT_PIXEL,
    .scaling = NCSCALE_SCALE,
    .flags = NCVISUAL_OPTION_NODEGRADE | NCVISUAL_OPTION_HORALIGNED,
  };
  struct ncplane* n = ncvisual_render(nc, ncv, &vopts);
  ncvisual_destroy(ncv);
  DEMO_RENDER(nc);
  demo_nanosleep(nc, &demodelay);
  ncplane_destroy(n);
  return 0;
}

static struct ncplane*
orcashow(struct notcurses* nc, int dimy, int dimx){
  char* path = find_data("natasha-blur.png");
  if(path == NULL){
    return NULL;
  }
  struct ncvisual* ncv = ncvisual_from_file(path);
  free(path);
  if(ncv == NULL){
    return NULL;
  }
  struct ncvisual_options vopts = {
    .blitter = NCBLIT_PIXEL,
    .flags = NCVISUAL_OPTION_NODEGRADE,
  };
  struct ncplane* n = ncvisual_render(nc, ncv, &vopts);
  ncvisual_destroy(ncv);
  int odimy, odimx, oy, ox;
  ncplane_yx(n, &oy, &ox);
  ncplane_dim_yx(n, &odimy, &odimx);
  if(odimy > dimy - 2){
    ncplane_destroy(n);
    return NULL;
  }
  if(odimx > dimx){
    ncplane_destroy(n);
    return NULL;
  }
  ncplane_move_yx(n, dimy - odimy - 2, dimx - odimx);
  return n;
}

static int
orcaride(struct notcurses* nc, struct ncplane* on){
  int odimy, odimx, oy, ox, dimx;
  ncplane_dim_yx(notcurses_stdplane(nc), NULL, &dimx);
  ncplane_yx(on, &oy, &ox);
  ncplane_dim_yx(on, &odimy, &odimx);
  if(ox >= (dimx - odimx) / 4){
    ox -= (dimx - odimx) / 4;
  }
  if(ncplane_move_yx(on, oy, ox)){
    return -1;
  }
  DEMO_RENDER(nc);
  return 0;
}

int intro(struct notcurses* nc){
  if(!notcurses_canutf8(nc)){
    return 0;
  }
  int rows, cols;
  struct ncplane* ncp = notcurses_stddim_yx(nc, &rows, &cols);
  uint32_t ccul, ccur, ccll, cclr;
  ccul = ccur = ccll = cclr = 0;
  channel_set_rgb8(&ccul, 0, 0xff, 0xff);
  channel_set_rgb8(&ccur, 0xff, 0xff, 0);
  channel_set_rgb8(&ccll, 0xff, 0, 0);
  channel_set_rgb8(&cclr, 0, 0, 0xff);
  // we use full block rather+fg than space+bg to conflict less with the menu
  ncplane_home(ncp);
  if(ncplane_highgradient_sized(ncp, ccul, ccur, ccll, cclr, rows, cols) <= 0){
    return -1;
  }
  nccell c = CELL_TRIVIAL_INITIALIZER;
  cell_set_bg_rgb8(&c, 0x20, 0x20, 0x20);
  ncplane_set_base_cell(ncp, &c);
  nccell ul = CELL_TRIVIAL_INITIALIZER, ur = CELL_TRIVIAL_INITIALIZER;
  nccell ll = CELL_TRIVIAL_INITIALIZER, lr = CELL_TRIVIAL_INITIALIZER;
  nccell hl = CELL_TRIVIAL_INITIALIZER, vl = CELL_TRIVIAL_INITIALIZER;
  if(ncplane_cursor_move_yx(ncp, 1, 0)){
    return -1;
  }
  if(cells_rounded_box(ncp, NCSTYLE_BOLD, 0, &ul, &ur, &ll, &lr, &hl, &vl)){
    return -1;
  }
  cell_set_fg_rgb(&ul, 0xff0000); cell_set_bg_rgb(&ul, 0x002000);
  cell_set_fg_rgb(&ur, 0x00ff00); cell_set_bg_rgb(&ur, 0x002000);
  cell_set_fg_rgb(&ll, 0x0000ff); cell_set_bg_rgb(&ll, 0x002000);
  cell_set_fg_rgb(&lr, 0xffffff); cell_set_bg_rgb(&lr, 0x002000);
  if(ncplane_box_sized(ncp, &ul, &ur, &ll, &lr, &hl, &vl, rows - 1, cols,
                       NCBOXGRAD_TOP | NCBOXGRAD_BOTTOM |
                        NCBOXGRAD_RIGHT | NCBOXGRAD_LEFT)){
    return -1;
  }
  uint64_t cul, cur, cll, clr;
  cul = cur = cll = clr = 0;
  channels_set_fg_rgb8(&cul, 200, 0, 200); channels_set_bg_rgb8(&cul, 0, 64, 0);
  channels_set_fg_rgb8(&cur, 200, 0, 200); channels_set_bg_rgb8(&cur, 0, 64, 0);
  channels_set_fg_rgb8(&cll, 200, 0, 200); channels_set_bg_rgb8(&cll, 0, 128, 0);
  channels_set_fg_rgb8(&clr, 200, 0, 200); channels_set_bg_rgb8(&clr, 0, 128, 0);
  centercols = cols > 80 ? 72 : cols - 8;
  if(ncplane_cursor_move_yx(ncp, 5, (cols - centercols) / 2 + 1)){
    return -1;
  }
  if(ncplane_gradient(ncp, "Δ", 0, cul, cur, cll, clr, rows - 8, cols / 2 + centercols / 2 - 1) <= 0){
    return -1;
  }
  cell_set_fg_rgb(&lr, 0xff0000); cell_set_bg_rgb(&lr, 0x002000);
  cell_set_fg_rgb(&ll, 0x00ff00); cell_set_bg_rgb(&ll, 0x002000);
  cell_set_fg_rgb(&ur, 0x0000ff); cell_set_bg_rgb(&ur, 0x002000);
  cell_set_fg_rgb(&ul, 0xffffff); cell_set_bg_rgb(&ul, 0x002000);
  if(ncplane_cursor_move_yx(ncp, 4, (cols - centercols) / 2)){
    return -1;
  }
  if(ncplane_box_sized(ncp, &ul, &ur, &ll, &lr, &hl, &vl, rows - 11, centercols,
                       NCBOXGRAD_TOP | NCBOXGRAD_BOTTOM | NCBOXGRAD_RIGHT | NCBOXGRAD_LEFT)){
    return -1;
  }
  cell_release(ncp, &ul); cell_release(ncp, &ur);
  cell_release(ncp, &ll); cell_release(ncp, &lr);
  cell_release(ncp, &hl); cell_release(ncp, &vl);
  const char s1[] = " Die Welt ist alles, was der Fall ist. ";
  const char str[] = " Wovon man nicht sprechen kann, darüber muss man schweigen. ";
  if(ncplane_set_fg_rgb8(ncp, 192, 192, 192)){
    return -1;
  }
  if(ncplane_set_bg_rgb8(ncp, 0, 40, 0)){
    return -1;
  }
  if(ncplane_putstr_aligned(ncp, rows / 2 - 4, NCALIGN_CENTER, s1) < 0){
    return -1;
  }
  ncplane_on_styles(ncp, NCSTYLE_ITALIC | NCSTYLE_BOLD);
  if(ncplane_putstr_aligned(ncp, rows / 2 - 3, NCALIGN_CENTER, str) < 0){
    return -1;
  }
  ncplane_off_styles(ncp, NCSTYLE_ITALIC);
  ncplane_set_fg_rgb8(ncp, 0xff, 0xff, 0xff);
  int major, minor, patch, tweak;
  notcurses_version_components(&major, &minor, &patch, &tweak);
  if(tweak){
    if(ncplane_printf_aligned(ncp, rows / 2 - 1, NCALIGN_CENTER, "notcurses %d.%d.%d.%d. press 'q' to quit.",
                              major, minor, patch, tweak) < 0){
      return -1;
    }
  }else{
    if(ncplane_printf_aligned(ncp, rows / 2 - 1, NCALIGN_CENTER, "notcurses %d.%d.%d. press 'q' to quit.",
                              major, minor, patch) < 0){
      return -1;
    }
  }
  ncplane_off_styles(ncp, NCSTYLE_BOLD);
  const wchar_t nwstr[] = L"▏▁ ▂ ▃ ▄ ▅ ▆ ▇ █ █ ▇ ▆ ▅ ▄ ▃ ▂ ▁▕";
  if(ncplane_putwstr_aligned(ncp, rows / 2 - 6, NCALIGN_CENTER, nwstr) < 0){
    return -1;
  }
  const wchar_t* iwstr;
  if(notcurses_cansextant(nc)){
    iwstr = L"▏▔ 🮂 🮃 ▀ 🮄 🮅 🮆 █ █ 🮆 🮅 🮄 ▀ 🮃 🮂 ▔▕";
  }else{
    iwstr = L"▏█ ▇ ▆ ▅ ▄ ▃ ▂ ▁ ▁ ▂ ▃ ▄ ▅ ▆ ▇ █▕";
  }
  if(ncplane_putwstr_aligned(ncp, rows / 2 + 1, NCALIGN_CENTER, iwstr) < 0){
    return -1;
  }
  if(rows < 45){
    ncplane_set_fg_rgb8(ncp, 0xc0, 0x80, 0x80);
    ncplane_set_bg_rgb8(ncp, 0x20, 0x20, 0x20);
    ncplane_on_styles(ncp, NCSTYLE_BLINK); // heh FIXME replace with pulse
    if(ncplane_putstr_aligned(ncp, 2, NCALIGN_CENTER, "demo runs best with at least 45 lines") < 0){
      return -1;
    }
    ncplane_off_styles(ncp, NCSTYLE_BLINK); // heh FIXME replace with pulse
  }
  struct ncplane* on = NULL;
  if(notcurses_check_pixel_support(nc) && notcurses_canopen_images(nc)){
    on = orcashow(nc, rows, cols);
  }
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  // there ought be 20 iterations
  uint64_t deadline = timespec_to_ns(&now) + timespec_to_ns(&demodelay) * 2;
  struct timespec iter;
  timespec_div(&demodelay, 10, &iter);
  int flipmode = 0;
  do{
    int err;
    if( (err = animate(nc, ncp, &flipmode)) ){
      return err;
    }
    demo_nanosleep(nc, &iter);
    clock_gettime(CLOCK_MONOTONIC, &now);
    if(on){
      if(flipmode % 4 == 0){
        orcaride(nc, on);
      }
    }
  }while(timespec_to_ns(&now) < deadline);
  ncplane_destroy(on);
  if(notcurses_check_pixel_support(nc) && notcurses_canopen_images(nc)){
    int err;
    if((err = greatscott(nc))){
      return err;
    }
  }
  if(notcurses_canfade(nc)){
    struct timespec fade = demodelay;
    int err;
    if( (err = ncplane_fadeout(ncp, &fade, demo_fader, NULL)) ){
      return err;
    }
  }
  return 0;
}
