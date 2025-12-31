import React from 'react';
import i18next from 'i18next';

const LanguageSwitch = () => {
    const languages = [
        {
            code: 'pl',
            name: 'Polski',
            country_code: 'pl'
        },
        {
            code: 'gb',
            name: 'English',
            country_code: 'gb'
        },
    ]

    const handleChangeLanguage = (language) => {
        i18next.changeLanguage(language.code);
    }
    return (
        <div className="p-3 d-flex">
            {languages.map((language) => (
                <div className={`p-3 d-flex container_lang ${i18next.language === language.code ? "selected_lang" : ""}`}
                    key={language.code} onClick={() => handleChangeLanguage(language)}>
                    <span className={`flag-icon flag-icon-${language.country_code}`}></span>
                    <span className="p-1">{language.name}</span>
                </div>
            ))}

        </div>
    )
}

export default LanguageSwitch;