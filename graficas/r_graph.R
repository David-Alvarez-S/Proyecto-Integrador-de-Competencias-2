library(ggplot2)
library(viridis)
library(dplyr)

# ==============================================================================
# CARGA DE DATOS (igual que antes)
# ==============================================================================
home_dir <- Sys.getenv("HOME")
base_dir <- file.path(home_dir, "Proyectos/Proyecto-Integrador-de-Competencias-2/graficas")
ruta_csv <- file.path(base_dir, "torques_plano_rz.csv")
if (!file.exists(ruta_csv)) ruta_csv <- file.path(base_dir, "torques_espacio_trabajo.csv")

datos_brazo <- read.csv(ruta_csv)
dir_destino <- paste0(base_dir, "/")
if (!dir.exists(dir_destino)) dir.create(dir_destino, recursive = TRUE)

TAU_MAX_MG90S  <- 0.1765
TAU_MAX_MG996R <- 0.9200

col_x <- names(datos_brazo)[1]
col_z <- names(datos_brazo)[2]

datos_procesados <- datos_brazo %>%
  mutate(
    pct_hombro = (tau2_Nm / TAU_MAX_MG996R) * 100,
    pct_codo   = (tau3_Nm / TAU_MAX_MG90S) * 100,
    pct_muneca = (tau4_Nm / TAU_MAX_MG90S) * 100,
    pct_total_critico = pmax(pct_hombro, pct_codo, pct_muneca)
  )

# ==============================================================================
# TEMA PERSONALIZADO
# ==============================================================================
tema_mapa <- theme_minimal(base_size = 13, base_family = "sans") +
  theme(
    plot.title       = element_text(face = "bold", size = 16, margin = margin(b = 4)),
    plot.subtitle    = element_text(color = "grey35", size = 11, margin = margin(b = 12)),
    plot.caption     = element_text(color = "grey50", size = 9, hjust = 0),
    plot.background  = element_rect(fill = "white", color = NA),
    panel.grid       = element_blank(),
    panel.border     = element_rect(color = "grey30", fill = NA, linewidth = 0.6),
    axis.title       = element_text(face = "bold", size = 11),
    axis.ticks       = element_line(color = "grey30"),
    axis.ticks.length = unit(3, "pt"),
    legend.position  = "right",
    legend.title     = element_text(face = "bold", size = 10),
    plot.margin      = margin(15, 15, 10, 15)
  )

# ==============================================================================
# FUNCIÓN GENERADORA DE MAPAS
# ==============================================================================
crear_mapa <- function(datos, var, titulo, subtitulo, nombre_leyenda = "Exigencia (%)",
                       umbral = 80, suavizar = TRUE) {
  
  max_val <- max(datos[[var]], na.rm = TRUE)
  limite_sup <- max(100, ceiling(max_val / 10) * 10)
  
  p <- ggplot(datos, aes(x = .data[[col_x]], y = .data[[col_z]]))
  
  # Relleno: raster (con interpolación opcional) en lugar de tiles
  p <- p + geom_raster(aes(fill = .data[[var]]), interpolate = suavizar)
  
  p <- p +
    # Isolíneas finas cada 20 %, sutiles
    geom_contour(aes(z = .data[[var]]), breaks = seq(20, 100, by = 20),
                 color = "white", alpha = 0.55, linewidth = 0.3) +
    # Umbral crítico: halo blanco debajo + línea roja encima (visible sobre cualquier color)
    geom_contour(aes(z = .data[[var]]), breaks = umbral,
                 color = "white", linewidth = 2, lineend = "round") +
    geom_contour(aes(z = .data[[var]]), breaks = umbral,
                 color = "#C8102E", linewidth = 1, linetype = "dashed") +
    # Si hay zonas que superan el 100 % (servo saturado), marcarlas
    geom_contour(aes(z = .data[[var]]), breaks = 100,
                 color = "black", linewidth = 0.9)
  
  p <- p +
    scale_fill_viridis_c(
      option = "inferno", direction = -1, 
      name = nombre_leyenda,
      limits = c(0, limite_sup),
      breaks = seq(0, limite_sup, length.out = 6), 
      guide = guide_colorbar(
        barheight = unit(8, "cm"), barwidth = unit(0.5, "cm"),
        ticks.colour = "white", frame.colour = "grey30", frame.linewidth = 0.4
      )
    ) +
    scale_x_continuous(expand = c(0, 0)) +
    scale_y_continuous(expand = c(0, 0)) +
    labs(
      title = titulo,
      subtitle = subtitulo,
      x = "Alcance radial R (m)",
      y = "Altura Z (m)",
      caption = paste0("Línea roja discontinua: ", umbral, " % de exigencia  |  ",
                       "Línea negra: 100 % (torque stall)")
    ) +
    coord_fixed() +
    tema_mapa
  
  p
}

guardar <- function(p, nombre) {
  ggsave(file.path(dir_destino, nombre), plot = p, width = 8.5, height = 6.5, dpi = 300, bg = "white")
}

# ==============================================================================
# FIGURAS
# ==============================================================================
p1 <- crear_mapa(datos_procesados, "pct_hombro",
                 "Mapa de exigencia estática — Hombro",
                 paste0("Porcentaje respecto al torque stall del MG990R (", TAU_MAX_MG996R, " N·m)"))
guardar(p1, "figura_1_torque_hombro.png")

p2 <- crear_mapa(datos_procesados, "pct_codo",
                 "Mapa de exigencia estática — Codo",
                 paste0("Porcentaje respecto al torque stall del MG90S (", TAU_MAX_MG90S, " N·m)"))
guardar(p2, "figura_2_torque_codo.png")

p3 <- crear_mapa(datos_procesados, "pct_muneca",
                 "Mapa de exigencia estática — Muñeca",
                 paste0("Porcentaje respecto al torque stall del MG90S (", TAU_MAX_MG90S, " N·m)"))
guardar(p3, "figura_3_torque_muneca.png")

p4 <- crear_mapa(datos_procesados, "pct_total_critico",
                 "Mapa de exigencia crítica global del brazo",
                 "Máximo esfuerzo relativo entre todos los actuadores en cada punto",
                 nombre_leyenda = "Exigencia\nmáxima (%)")
guardar(p4, "figura_4_torque_global_critico.png")

list.files(dir_destino, pattern = "\\.png$")