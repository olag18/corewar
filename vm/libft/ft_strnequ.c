/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnequ.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clanier <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/11/05 16:33:54 by clanier           #+#    #+#             */
/*   Updated: 2016/11/07 07:18:04 by clanier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

int	ft_strnequ(char const *s1, char const *s2, size_t n)
{
	size_t	i;

	if (s1 && s2)
	{
		i = 0;
		while (*s1 == *s2 && *s1 && *s2 && i < n)
		{
			s1++;
			s2++;
			i++;
		}
		if (*s1 == *s2 || i == n)
			return (1);
	}
	return (0);
}