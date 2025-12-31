import React from 'react';
import ReactDOM from 'react-dom';
import App from './App';
import "./styles/main.scss";
import "./styles/_dark/main.scss";
import * as serviceWorker from './serviceWorker';
import i18n from "i18next";
import { initReactI18next } from "react-i18next";
import LanguageDetector from 'i18next-browser-languagedetector';
import 'flag-icon-css/css/flag-icons.css';

import pl from './assets/locales/pl';
import gb from './assets/locales/gb';

i18n
    .use(initReactI18next) // passes i18n down to react-i18next
    .use(LanguageDetector)
    .init({
        supportedLngs: ['pl', 'gb'],

        resources: {
            pl: { translation: pl },
            gb: { translation: gb },
        },

        debug: false,
        fallbackLng: "pl",
        detection: {
            order: ['cookie', 'htmlTag', 'localStorage', 'path', 'subdomain'],
            caches: ['cookie']
        },
        react: {
            useSuspense: false
        },

    },
    );

ReactDOM.render(<App />, document.getElementById('root'));

// If you want your app to work offline and load faster, you can change
// unregister() to register() below. Note this comes with some pitfalls.
// Learn more about service workers: https://bit.ly/CRA-PWA
serviceWorker.unregister();
