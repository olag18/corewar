#include "corewar.h"

void	ft_display_mars(t_mars **mars, t_cpu *cpu)
{
	if ((*mars)->opt & 0b00000001)
		ft_print_mars(mars, cpu);
	else if ((*mars)->opt & 0b00000010)
		ft_refresh_mars(mars, cpu);
	else if ((*mars)->opt & 0b00000100)
		ft_ncurses_mars(*mars, cpu);
	usleep(10000);
	(*mars)->modif = false;
}
