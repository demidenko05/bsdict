/* BSD 2-Clause License
Copyright (c) 2020, Beigesoft™
All rights reserved.
See the LICENSE in the root source folder */
 
/**
 * <p>Beigesoft™ I18N library.</p>
 * @author Yury Demidenko
 **/

#ifndef BSI18N_H
#define BSI18N_H 1

/**
 * <p>Get I18N message for current/chosen locale.</p>
 * @param pKey key
 * @return I18N message or pKey if error.
 **/
char *bsi18n_msg(char *pKey);

/**
 * <p>Get I18N message for given locale.</p>
 * @param pKey key
 * @param pLocale locale
 * @return I18N message or pKey if error.
 **/
char *bsi18n_msg_for(char *pKey, char *pLocale);

/**
 * <p>Set current/chosen locale.</p>
 * @param pLocale locale
 **/
void bsi18n_set_locale(char *pLocale);

#endif
