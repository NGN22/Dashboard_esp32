async function actualizar()
{
    try
    {
        const respuesta =
            await fetch("/api/state");

        const datos =
            await respuesta.json();

        actualizarWifi(datos);
        actualizarAlarma(datos);

        actualizarCamposPendientes(datos);
    }
    catch(error)
    {
        console.error(error);
    }
}

/************************************************
 * WIFI
 ************************************************/

function actualizarWifi(datos)
{
    const wifi =
        document.getElementById("wifi");

    wifi.textContent =
        datos.wifi;

    wifi.className = "status";

    if (datos.wifi === "CONECTADO")
    {
        wifi.classList.add("green");
    }
    else if (datos.wifi === "CONECTANDO")
    {
        wifi.classList.add("orange");
    }
    else
    {
        wifi.classList.add("red");
    }
}

/************************************************
 * ALARMA
 ************************************************/

function actualizarAlarma(datos)
{
    const alarma =
        document.getElementById("alarma");

    alarma.textContent =
        datos.alarma;

    alarma.className = "status";

    if (datos.alarma === "DESACTIVADA")
    {
        alarma.classList.add("green");
    }
    else if (datos.alarma === "ACTIVADA")
    {
        alarma.classList.add("orange");
    }
    else
    {
        alarma.classList.add("red");
    }
}

/************************************************
 * CAMPOS FUTUROS
 ************************************************/

function actualizarCamposPendientes(datos)
{
    const temperatura =
        document.getElementById("temperatura");

    const humedad =
        document.getElementById("humedad");

    const iluminacion =
        document.getElementById("iluminacion");

    const aire =
        document.getElementById("aire");

    const tomacorrientes =
        document.getElementById("tomacorrientes");

    if (datos.temperatura !== undefined)
    {
        temperatura.textContent =
            `${datos.temperatura} °C`;

        temperatura.className =
            "status blue";
    }

    if (datos.humedad !== undefined)
    {
        humedad.textContent =
            `${datos.humedad} %`;

        humedad.className =
            "status blue";
    }

    if (datos.iluminacion !== undefined)
    {
        iluminacion.textContent =
            datos.iluminacion ?
            "ENCENDIDA" :
            "APAGADA";

        iluminacion.className =
            datos.iluminacion
            ? "status green"
            : "status red";
    }

    if (datos.aire_acondicionado !== undefined)
    {
        aire.textContent =
            datos.aire_acondicionado ?
            "ENCENDIDO" :
            "APAGADO";

        aire.className =
            datos.aire_acondicionado
            ? "status green"
            : "status red";
    }

    if (datos.tomacorrientes !== undefined)
    {
        tomacorrientes.textContent =
            datos.tomacorrientes ?
            "ACTIVOS" :
            "INACTIVOS";

        tomacorrientes.className =
            datos.tomacorrientes
            ? "status green"
            : "status red";
    }
}

actualizar();

setInterval(
    actualizar,
    1000);