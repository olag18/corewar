#include "corewar.h"

t_cpu	*ft_get_cycle_wait(t_cpu *cpu, unsigned char opcode)
{
	if (cpu->cycle_wait)
	{
		--cpu->cycle_wait;
		return (cpu);
	}
	if (opcode == 0x01 || opcode == 0x04 || opcode == 0x05 || opcode == 0x0d)
		cpu->cycle_wait = 10;
	else if (opcode == 0x02 || opcode == 0x03)
		cpu->cycle_wait = 5;
	else if (opcode == 0x06 || opcode == 0x07 || opcode == 0x08)
		cpu->cycle_wait = 6;
	else if (opcode == 0x09)
		cpu->cycle_wait = 20;
	else if (opcode == 0x0a || opcode == 0x0b)
		cpu->cycle_wait = 25;
	else if (opcode == 0x0c)
		cpu->cycle_wait = 800;
	else if (opcode == 0x0e)
		cpu->cycle_wait = 50;
	else if (opcode == 0x0f)
		cpu->cycle_wait = 1000;
	return (cpu);
}

t_cpu	*ft_get_opcode(t_mars **mars, t_cpu *cpu, int opcode)
{
	if (!cpu->cycle_wait && opcode == 0x01)
		cpu = ft_live(mars, cpu);
	else if (!cpu->cycle_wait && (opcode == 0x02 || opcode == 0x0d))
		cpu = ft_ld(mars, cpu, opcode);
	else if (!cpu->cycle_wait && opcode == 0x03)
		cpu = ft_st(mars, cpu);
	else if (!cpu->cycle_wait && (opcode == 0x04 || opcode == 0x05))
		cpu = ft_math(mars, cpu, opcode);
	else if (!cpu->cycle_wait && (opcode == 0x06
	|| opcode == 0x07 || opcode == 0x08))
		cpu = ft_binary_op(mars, cpu, opcode);
	else if (!cpu->cycle_wait && opcode == 0x09)
		cpu = ft_zjmp(mars, cpu);
	else if (!cpu->cycle_wait && opcode == 0x0b)
		cpu = ft_sti(mars, cpu);
	else if (!cpu->cycle_wait && (opcode == 0x0a || opcode == 0x0e))
		cpu = ft_ldi(mars, cpu, opcode);
	else if (!cpu->cycle_wait)
		cpu->pc++;
	return (cpu);
}

void	ft_run_cpu(t_mars **mars, t_cpu **cpu)
{
	t_cpu 		*lst;
	unsigned char	opcode;
	int		fork_address;

	lst = *cpu;
	(*mars)->process = 0;
	while (lst)
	{
		while (lst && lst->live < 0)
			lst = lst->next;
		if (!lst)
			break ;
		opcode = (*mars)->arena[lst->pc % MEM_SIZE];
		lst = ft_get_cycle_wait(lst, opcode);
		if (!lst->cycle_wait && (opcode == 0x0c || opcode == 0x0f))
		{
			lst = ft_fork(mars, lst, &fork_address, (opcode));
			ft_cpu_cpyfront(cpu, lst, lst->pc + fork_address);
		}
		else
			lst = ft_get_opcode(mars, lst, opcode);
		lst = lst->next;
		++(*mars)->process;
	}
}

int		ft_kill_cpu(t_mars **mars, t_cpu **cpu)
{
	t_cpu	*lst;
	int		nlive;

	nlive = 0;
	lst = *cpu;
	if ((*mars)->cycle_to_die <= 0)
		return (0);
	while (lst)
	{
		if (lst->live < 1)
			lst->live = -1;
		else if (nlive += lst->live)
			lst->live = 0;
		lst = lst->next;
	}
	++(*mars)->live_check;
	if (nlive >= NBR_LIVE || (*mars)->live_check == MAX_CHECKS)
	{
		(*mars)->live_check = 0;
		(*mars)->cycle_to_die -= CYCLE_DELTA;
		if ((*mars)->opt & 0b00010000)
			ft_printf("cycle to die is now %d\n", (*mars)->cycle_to_die);
	}
	(*mars)->last_live_check = 0;
	return (nlive);
}

void	ft_draw_loading_mars(t_mars **mars, int wait)
{
	int	i;
	int	j;
	int	ij;

	i = -1;
	while (++i < 64)
	{
		j = -1;
		while (++j < 64)
		{
			ij = i * 64 + j;
			if ((*mars)->arena_color[ij])
			{
				ft_draw_case(*mars, i, j,
				ft_get_color((*mars)->arena_color[ij]));
				mlx_put_image_to_window((*mars)->mlx->mlx,
				(*mars)->mlx->win, (*mars)->mlx->img, 0, 0);
				usleep(wait);
			}
		}
	}
}

void	ft_play_sound(t_mars *mars)
{
	mars->sound = (t_sound*)malloc(sizeof(t_sound));
	mars->sound->system = NULL;
	FMOD_System_Create(&mars->sound->system);
	FMOD_System_Init(mars->sound->system, 2, FMOD_INIT_NORMAL, NULL);
	FMOD_System_CreateSound(mars->sound->system, "music/starfish.mp3",
	FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, 0, &mars->sound->sound);
	FMOD_Sound_SetLoopCount(mars->sound->sound, -1);
	FMOD_System_PlaySound(mars->sound->system,
	mars->sound->sound, NULL, 0, NULL);
	FMOD_System_GetMasterChannelGroup(mars->sound->system,
	&mars->sound->channelgroup);
}

void	ft_music_load(t_mars **mars, t_cpu *cpu)
{
	(*mars)->total_size = 0;
	while (cpu)
	{
		(*mars)->total_size += cpu->size;
		cpu = cpu->next;
	}
	ft_play_sound(*mars);
	ft_draw_loading_mars(mars, 18500000 / (*mars)->total_size);
	mlx_put_image_to_window((*mars)->mlx->mlx,
	(*mars)->mlx->win, (*mars)->mlx->img, 0, 0);
	usleep(300000);
}

void	ft_run_mars(t_mars *mars, t_cpu *cpu)
{
	char	dump;

	dump = 0;
	if ((mars->opt & 0b01000010) == 0b01000010)
		ft_music_load(&mars, cpu);
	while (42)
	{
		if (mars->dump == mars->cycle && (dump = 1))
			break ;
		if (mars->opt & 0b00000111 && mars->modif
		&& mars->last_flush + 15 < mars->cycle
		&& (mars->last_flush = mars->cycle))
			ft_display_mars(&mars, cpu);
		ft_run_cpu(&mars, &cpu);
		++mars->cycle;
		++mars->last_live_check;
		if (mars->opt & 0b10000)
			ft_printf("It's now cycle %d\n", mars->cycle);
		if (mars->last_live_check >= mars->cycle_to_die)
			if (!ft_kill_cpu(&mars, &cpu))
				break ;
	}
	if (!(mars->opt & 0b00000100))
		!dump ? ft_print_winner(mars, cpu) : ft_dump_memory(mars->arena);
	ft_del_cpu(&cpu);
}
